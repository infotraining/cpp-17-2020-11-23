#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> oi1;
    REQUIRE(oi1.has_value() == false);

    std::optional<int> oi2 = std::nullopt;
    REQUIRE(oi2.has_value() == false);

    std::optional<int> oi3 = 42;
    REQUIRE(oi3.has_value());

    std::optional oi4 = 665;
    REQUIRE(oi4.has_value());

    std::optional<std::string> os1 = std::string(10, 'a');
    std::optional<std::string> os2(std::in_place, 10, 'a');

    if (os1)
    {
        std::cout << *os1 << "\n";
    }

    SECTION("access to value")
    {
        optional val = 42;

        SECTION("unsafe")
        {
            REQUIRE(*val == 42);
        }

        SECTION("safe")
        {
            REQUIRE(val.value() == 42);

            val.reset();
            val = std::nullopt;

            REQUIRE_THROWS_AS(val.value(), std::bad_optional_access);
        }
    }
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if (const char* x = std::getenv(n))
        return x;
    else
        return std::nullopt;
}

TEST_CASE("maybe_getenv")
{
    std::cout << maybe_getenv("NOPATH").value_or("(NONE)") << "\n";
}

TEST_CASE("optional & move semantics")
{
    optional ov = vector {1, 2, 3};

    REQUIRE(ov.value() == vector {1, 2, 3});

    optional otarget = std::move(ov);

    REQUIRE(ov.has_value());
    REQUIRE(otarget.value() == vector {1, 2, 3});
}

TEST_CASE("strange cases")
{
    SECTION("optional<bool>")
    {
        optional<bool> ob {false};

        if (ob)
        {
            std::cout << "ob has value..." << *ob << "\n";
        }

        REQUIRE(ob == false);
    }

    SECTION("optional<int*>")
    {
        optional<int*> op{nullptr};

        if (op)
        {
            std::cout << "op has value..." << *op << "\n";
        }

        REQUIRE(op == nullptr);
    }
}

// TODO

TEST_CASE("to_int")
{
    optional n = to_int("42");

    REQUIRE(n.has_value());
    REQUIRE(*n == 42);

    optional ne = to_int("4b");
    REQUIRE(n.has_value() == false);
}