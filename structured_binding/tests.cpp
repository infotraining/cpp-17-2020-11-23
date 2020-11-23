#include "catch.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <new>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

using namespace std;
using namespace std::literals;

namespace BeforeCpp17
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename TContainer>
tuple<int, int, double> calc_stats(const TContainer& data)
{
    auto [min_pos, max_pos] = minmax_element(std::begin(data), std::end(data));

    double avg = accumulate(std::begin(data), std::end(data), 0.0) / std::size(data);

    return tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    SECTION("reading tuple using get")
    {
        auto results = calc_stats(data);

        REQUIRE(std::get<0>(results) == 1); // min
        REQUIRE(std::get<1>(results) == 665); // max
    }

    SECTION("reading using std::tie")
    {
        int min, max;
        double avg;

        tie(min, max, avg) = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }

    SECTION("using structured bindings")
    {
        const auto [min, max, avg] = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }

    SECTION("calc_stats & native array")
    {
        int dataset[] = {5, 23, 665, 42, 0, 434};

        const auto [min, max, avg] = calc_stats(dataset);

        REQUIRE(min == 0);
        REQUIRE(max == 665);
    }
}

struct ErrorCode
{
    const int errc;
    const char* m;
};

ErrorCode open_file(const char* filename)
{
    return ErrorCode {13, "Error#13"};
}

TEST_CASE("Structured bindings - details")
{
    SECTION("native arrays")
    {
        int coord[4] = {1, 2, 3, 4};

        auto [x, y, z, t] = coord;

        REQUIRE(x == 1);
    }

    SECTION("std arrays")
    {
        std::array coord = {1, 2, 3, 4};

        alignas(128) auto [x, y, z, t] = coord;

        REQUIRE(x == 1);
    }

    SECTION("std::pair")
    {
        std::set<int> numbers;

        auto [pos, was_inserted] = numbers.insert(6);
        REQUIRE(*(pos) == 6);
        REQUIRE(was_inserted == true);
    }

    SECTION("struct")
    {
        auto [error_code, message] = open_file("data.dat");
        REQUIRE(error_code == 13);
        REQUIRE(message == "Error#13"s);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    Timestamp t1 {11, 45, 0};

    const auto& [hour, min, seconds] = t1;

    SECTION("works like this")
    {
        const auto& entity = t1;

        auto& hour = entity.h;
        auto& min = entity.m;
        auto& seconds = entity.s;
    }
}

struct Data
{
    int d1;
    char c;
    int d2;
};

TEST_CASE("alignas")
{
    int counter1 = 0;
    char c;
    // padding
    alignas(std::hardware_destructive_interference_size) int counter2 = 0;
}

TEST_CASE("stringlike literals")
{
    using namespace std::literals;

    auto text1 = "text"; // const char*
    auto text2 = "text"s; // std::string - C++14
    auto text3 = "text"sv; // std::string_view - C++17

    REQUIRE(text1 == text2);
    REQUIRE(text1 == text3);
}

TEST_CASE("structured bindings - use cases")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}, {3, "three"}};

    SECTION("iteration over maps")
    {    
        for(const auto& [key, value] : dict)
        {
            std::cout << key << ": " << value << "\n";
        }
    }

    SECTION("insert into associative containers")
    {
        auto [pos, was_inserted] = dict.insert(pair(1, "one"));
        
        REQUIRE(pos->second == "one"s);
        REQUIRE(was_inserted == false);
    }

    SECTION("init many vars")
    {
        list lst = {1, 2, 3};

        for(auto [index, pos] = tuple(0, begin(lst)); pos != end(lst); ++pos, ++index)
        {
            std::cout << index << " - " << *pos << "\n";
        }
    }
}

////////////////////////////////////////////
// tuple like protocol - Structured Bindings

enum Something
{
    some = 1, thing, other
};

const std::map<Something, std::string_view> something_desc = {{some, "some"sv}, {thing, "thing"sv}, {other, "other"sv}};

// step 1
template <>
struct std::tuple_size<Something>
{
    static constexpr size_t value = 2;
};

// step 2
template <>
struct std::tuple_element<0, Something>
{
    using type = int;
};

template <>
struct std::tuple_element<1, Something>
{
    using type = std::string_view;
};

// step 3
template <size_t Index>
decltype(auto) get(const Something&);

template <>
decltype(auto) get<0>(const Something& sth)
{
    return static_cast<int>(sth);
}

template <>
decltype(auto) get<1>(const Something& sth)
{
    return something_desc.at(sth);
}
 
TEST_CASE("user type & tuple like protocol")
{
    const Something sth = other;

    const auto [value, description] = sth;

    REQUIRE(value == 3);
    REQUIRE(description == "other"sv);
}