#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <optional>
#include <list>
#include <array>

#include "catch.hpp"

using namespace std;

void foo()
{
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

// void deduce1(Container auto arg)
// {
//     vector vec(begin(arg), end(arg));

//     for(auto&& item : vec)
//     {
//         std::cout << item << "\n";
//     }
// }

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    [[maybe_unused]] auto target = std::forward<T>(arg);
    puts(__PRETTY_FUNCTION__);
}

// auto deduce1(auto arg1, auto arg2) -> enable_if_t<is_same_v<decltype(arg1), decltype(arg2)>>
// {
//     puts(__PRETTY_FUNCTION__);
// }

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto ax1 = 10; // int
    deduce1(10);

    [[maybe_unused]] auto ax2 = cx; // int
    deduce1(cx);

    [[maybe_unused]] auto ax3 = ref_x; // int
    deduce1(ref_x);

    [[maybe_unused]] auto ax4 = cref_x; // int
    deduce1(cref_x);

    [[maybe_unused]] auto ax5 = tab; // int*
    deduce1(tab);

    [[maybe_unused]] auto ax6 = foo; // void(*)()
    deduce1(foo);
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto& ax1 = x; // int&
    deduce2(x);

    [[maybe_unused]] auto& ax2 = cx; // const int&
    deduce2(cx);

    [[maybe_unused]] auto& ax3 = ref_x; // int&
    deduce2(ref_x);

    [[maybe_unused]] auto& ax4 = cref_x; // const int&
    deduce2(cref_x);

    [[maybe_unused]] auto& ax5 = tab; // int(&)[10]
    deduce2(tab);

    [[maybe_unused]] auto& ax6 = foo; // void(&)()
    deduce2(foo);
}

TEST_CASE("case 3")
{
    int x = 10;

    [[maybe_unused]] auto&& ax1 = x; // int&
    deduce3(x);

    [[maybe_unused]] auto&& ax2 = 10; // int&&
    deduce3(10);

    [[maybe_unused]] int&& rvref = 10;
    deduce3(std::move(rvref));
}

template <typename T1 = int, typename T2 = T1>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair() = default;

    ValuePair(const T1& f, const T2& s)
        : fst {f}
        , snd {s}
    {
    }
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*) -> ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    SECTION("Before C++17")
    {
        ValuePair<int, double> vp1 {1, 3.14};
    }

    SECTION("Since C++17")
    {
        ValuePair vp1 {1, 3.14};
        static_assert(is_same_v<ValuePair<int, double>, decltype(vp1)>);

        ValuePair vp2 {3, 5.5f};
        static_assert(is_same_v<ValuePair<int, float>, decltype(vp2)>);

        ValuePair vp3 {"text", "text"s};
        static_assert(is_same_v<ValuePair<const char*, std::string>, decltype(vp3)>);

        int tab[10] = {};
        ValuePair vp4 {foo, tab};
        static_assert(is_same_v<ValuePair<void (*)(), int*>, decltype(vp4)>);

        ValuePair vp5; // ValuePair<int, int>

        ValuePair vp6{"text", "abc"};
        static_assert(is_same_v<ValuePair<std::string, std::string>, decltype(vp6)>);
    }

    SECTION("CTAD - special case")
    {
        std::vector vec {1, 2, 3}; // vector<int>

        SECTION("copy or move syntax")
        {
            std::vector data1 {vec}; // SPECIAL CASE - vector<int>
            static_assert(is_same_v<std::vector<int>, decltype(data1)>);

            std::vector data2 {std::move(vec)}; // SPECIAL CASE - vector<int>
            static_assert(is_same_v<std::vector<int>, decltype(data2)>);
        }

        SECTION("other syntax")
        {
            std::vector data1 {vec, vec};
            static_assert(is_same_v<std::vector<std::vector<int>>, decltype(data1)>);
        }
    }
}

template <typename T1, typename T2>
struct Aggregate
{
    T1 id;
    T2 name;
};

template <typename T1, typename T2>
Aggregate(T1, T2) -> Aggregate<T1, T2>;

Aggregate(int, double) -> Aggregate<int, float>;

TEST_CASE("CTAD + aggregates")
{
    Aggregate agg1{1, "name"s};

    Aggregate agg2{1, 3.14};

    Aggregate agg3{'s', 3.14};
}

int add(int a, int b)
{
    return a + b;
}

TEST_CASE("CTAD + std library")
{
    SECTION("std::pair")
    {
        auto p1 = make_pair(1, 3.14);
        pair<int, double> p2(1, 3.14);

        pair p3(1, 3.14);
        auto p4 = pair(1, 3.14);

        int a[2], b[3];
        pair p{a, b}; // explicit deduction guide is used in this case
    }

    SECTION("std::tuple")
    {
        auto t0 = make_tuple(1, 3.14, "text"); // tuple<int, double, const char*>
        tuple t1(1, 3.14, "text"); // tuple<int, double, const char*>

        auto [x, y] = tuple(1, 3.14);

        pair p(1, 3.14);
        tuple t = p;
    }

    SECTION("std::optional")
    {
        std::optional<int> o1 = 42;
        std::optional o2 = 42; // optional<int>

        std::optional o3 = o2; // optional<int>
    }

    SECTION("smart pointers")
    {
        std::unique_ptr<int> ptr1(new int(13));
        std::unique_ptr ptr2 = std::make_unique<std::string>(10, 'a');

        std::shared_ptr<int> sptr1(new int(42));
        std::shared_ptr sptr2 = std::make_shared<std::string>(10, 'a');
        std::shared_ptr sptr3 = std::move(ptr1);

        std::weak_ptr wptr1 = sptr1;
        std::weak_ptr wptr2 = sptr2;
    }

    SECTION("std::function")
    {
        std::function<int(int, int)> f1 = add;
        std::function f2 = add;
        REQUIRE(f2(1, 2) == 3);

        std::function f3 = [](int a) { return 2 * a; };
        REQUIRE(f3(2) == 4);
    }

    SECTION("std containers")
    {
        vector vec1 = {1, 2, 3};

        vector vec2(3, "text"s);
        REQUIRE(vec2 == vector<string>{"text", "text", "text"});

        list lst(vec1.begin(), vec1.end()); // list<int>

        array arr1 = {1, 2, 3, 4, 5}; // std::array<int, 5>
    }
}

template <typename T, auto N>
struct Array
{
    T items[N];

    constexpr size_t size() const
    {
        return N;
    }
};

// deduction guide
template <typename Head, typename... Tail>
Array(Head, Tail...) 
    -> Array<enable_if_t<(is_same_v<Head, Tail> && ...), Head>, sizeof...(Tail) + 1>;

TEST_CASE("Array")
{
    Array<int, 3u> arr1 = {1, 2, 3};
    Array<int, 3l> arr2 = {1, 2, 3};

    static_assert(is_same_v<decltype(arr1), decltype(arr2)>);


    Array arr = {1, 2, 3};

    static_assert(is_same_v<Array<int, 3>, decltype(arr)>);
    static_assert(arr.size() == 3);

}

template <auto N>
struct Value
{
    constexpr static auto value{N};
};

TEST_CASE("auto as template parameter")
{
    auto v1 = Value<32u>::value;
    auto v2 = Value<'a'>::value;
    
}