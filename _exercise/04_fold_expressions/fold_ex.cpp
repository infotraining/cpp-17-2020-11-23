#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TContainer, typename... TArgs>
auto matches(const TContainer& container, const TArgs&... args)
{
    return (... + std::count(std::begin(container), std::end(container), args));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    vector<int> v {1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abcdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("aabcdef", 'a', 'd', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};

namespace Explain
{
    template <typename T>
    struct vector
    {
        void push_back(const T& item)
        {
            // by copy
        }

        void push_back(T&& item)
        {
            // by move
        }
    };
}

template <typename TContainer, typename T>
void insert_into_container(TContainer& vec, T&& item)
{
    vec.push_back(std::forward<T>(item));
}

TEST_CASE("vector & forward")
{
    string s = "abc";
    std::vector<string> v;
    v.push_back(s); // copy
    v.push_back(string("ABC")); // move

    insert_into_container(v, s); // copy
    insert_into_container(v, string("abc")); // move

    auto&& obj = string("abc");

    v.push_back(std::forward<decltype(obj)>(obj));
}

// TODO - ,
template <typename... TArgs>
auto make_vector(TArgs&&... args)
{
    using T = std::common_type_t<TArgs...>;

    std::vector<T> m_vec;
    m_vec.reserve(sizeof...(args));

    (m_vec.push_back(std::forward<TArgs>(args)), ...);
    return m_vec;
}

TEST_CASE("make_vector - create vector from a list of arguments")
{
    using namespace Catch::Matchers;

    SECTION("ints")
    {
        std::vector v = make_vector(1, 2, 3);

        REQUIRE_THAT(v, Equals(vector {1, 2, 3}));
    }

    SECTION("unique_ptrs")
    {
        auto ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string> {"a", "b", "a"}));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
struct Range
{
    T low;
    T high;

    Range(T low, T high)
        : low(low)
        , high(high)
    {
    }

    bool operator()(const T& item) const
    {
        return (low < item) && (item < high);
    }
};

// deduction guide
template <typename T1, typename T2>
Range(T1, T2) -> Range<std::common_type_t<T1, T2>>;

template <class TRange, typename... TArgs>
auto within(const TRange& in_range, const TArgs&... args)
{
    return (in_range(args) && ...);
}

TEST_CASE("within - checks if all values fit in range [low, high]")
{
    REQUIRE(within(Range {10, 20.0}, 1, 15, 30) == false);
    REQUIRE(within(Range {10, 20}, 11, 12, 13) == true);
    REQUIRE(within(Range {5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void hash_combine(size_t& seed, const T& value)
{
    seed ^= std::hash<T> {}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// TODO - ,
template <typename... TArgs>
size_t combined_hash(const TArgs&... args)
{
    size_t seed {};
    (..., hash_combine(seed, args));
    
    return seed;
}

TEST_CASE("combined_hash - write a function that calculates combined hash value for a given number of arguments")
{
    REQUIRE(combined_hash(1U) == 2654435770U);
    REQUIRE(combined_hash(1, 3.14, "string"s) == 10365827363824479057U);
    REQUIRE(combined_hash(123L, "abc"sv, 234, 3.14f) == 162170636579575197U);
}