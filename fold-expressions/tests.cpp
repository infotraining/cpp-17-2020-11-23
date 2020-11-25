#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

namespace BeforeCpp17
{
    ///////////////////////////////////////
    // sum

    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {        
        //return (tail...);
        return head + sum(tail...);     //  -> return 1 + sum(2, 3)
    }

    //////////////////////////////////////////
    // print

    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

// operator +
template <typename... TArgs>
auto sum(TArgs... args)
{
    return (... + args); // left fold +

    // sum(1, 2, 3, 4)
    //   -> return (((1 + 2) + 3) + 4)
}

template <typename... TArgs>
auto sum_right(TArgs... args)
{
    return (args + ...); // right fold +

    // sum_right(1, 2, 3, 4)
    //   -> return (((1 + (2 + (3 + 4)))
}

template <typename... TArgs>
void print(const TArgs&... args)
{
    bool is_first = true;
    auto with_space = [&is_first](const auto& arg) {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return arg;
    };

    (std::cout << ... << with_space(args)) << "\n"; // left binary fold <<
}

////////////////////////////////////
// all_true

template <typename... TArgs>
bool all_true(const TArgs&... args)
{
    return (args && ...);
}

/////////////////////////////////////
// print_lines

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    ((std::cout << args << "\n"), ...);
}

///////////////////////////////////
// call

void f(int x)
{
    std::cout << "f(" << x << ")\n";
}

template <typename TFunc, typename... TArgs>
void call(TFunc func, TArgs&&... args)
{
    (func(std::forward<TArgs>(args)), ...);
}

template <typename T, typename... TPtrs2MemberF>
void call_members(T& obj, TPtrs2MemberF... pm)
{
    ((obj.*pm)(), ...);
}

struct Foo
{
    void bar1() { std::cout << "Foo::bar1()\n"; }
    void bar2() { std::cout << "Foo::bar2()\n"; }
};

TEST_CASE("pointer to member")
{
    void (Foo::*pf)();

    pf = &Foo::bar2;

    Foo f;

    (f.*pf)();
}

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();

    print(1, 2.33, "text"s, "abc");
    print_lines(1, 2.33, "text"s, "abc");

    REQUIRE(sum(1, 2, 3, 4, 5) == 15);
    REQUIRE(sum(1, 2, 3.14, 4) == Approx(10.14));

    REQUIRE_FALSE(all_true(true, false, false, true));
    REQUIRE(all_true(true, 1, true, true));

    call(f, 1, 2, 3);
}