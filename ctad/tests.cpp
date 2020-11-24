#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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

TEST_CASE("CTAD + aggregates")
{
    Aggregate agg1{1, "name"s};
}