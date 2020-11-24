#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

int foo()
{
    static int x;

    return ++x;
}

TEST_CASE("expression evaluation order")
{
    std::map<int, int> dict;
    dict[0] = dict.size();

    for (const auto& [key, value] : dict)
    {
        std::cout << key << " - " << value << "\n";
    }

    std::cout << foo() << foo() << foo() << "\n";
}

enum class Coffee : uint8_t
{
    espresso,
    chemex,
    v60
};

TEST_CASE("init enums")
{
    Coffee c1 {1};

    REQUIRE(c1 == Coffee::chemex);
}

namespace Explain
{
    enum class byte : uint8_t
    {
    };

    template <class IntegerType>
    constexpr std::byte& operator<<=(std::byte& b, IntegerType shift) noexcept
    {
        return b <<= shift;
    }
}

TEST_CASE("byte")
{
    SECTION("legacy byte")
    {
        char byte1 = 0b0000111;

        byte1 += 2;

        byte1 <<= 2;

        std::cout << byte1 << "\n";
    }

    SECTION("C++17 std::byte")
    {
        std::byte byte2 {0b0000'1111};

        std::cout << std::to_integer<size_t>(byte2) << "\n";

        //byte2 += 2;
        byte2 <<= 2;
        byte2 = ~byte2;

        std::cout << std::to_integer<size_t>(byte2) << "\n";
    }
}

TEST_CASE("auto + {}")
{
    int a1 = 10;
    int a2 {10};

    auto b1 = 10; // int
    auto b2 {10}; // int - since C++17

    //auto b3{1, 2, 3}; // ill-formed
    auto b3 = {1, 2, 3}; // std::initializer_list<int>
}

void foo1()
{
    puts(__PRETTY_FUNCTION__);
}

void foo2() noexcept
{
    puts(__PRETTY_FUNCTION__);
}

template <typename Callable1, typename Callable2>
void call(Callable1 f1, Callable2 f2)
{
    f1();
    f2();
}

struct Base
{
    virtual ~Base() = default;
    virtual void foo() noexcept
    {
        puts(__PRETTY_FUNCTION__);
    }
};

struct Dervied : Base
{
    void foo() noexcept override
    {
        puts(__PRETTY_FUNCTION__);
    }
};

TEST_CASE("noexcept")
{
    void (*fptr)() noexcept = nullptr;

    fptr = foo2;
    //fptr = foo1;

    call(foo1, foo2);

    Dervied d;
    Base* ptr = &d;
    ptr->foo();
}