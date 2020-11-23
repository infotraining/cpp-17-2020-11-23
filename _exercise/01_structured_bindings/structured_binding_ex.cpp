#include <iostream>
#include <string>
#include <tuple>

#include "catch.hpp"

using namespace std;

enum class Bitfields : uint32_t
{
};

TEST_CASE("split integer to bytes")
{
    Bitfields value{0b00000001'11100010'00000100'01001000};

    auto [b1, b2, b3, b4] = value;

    CHECK(b1 == 0b00000001);
    CHECK(b2 == 0b11100010);
    CHECK(b3 == 0b00000100);
    CHECK(b4 == 0b01001000);
}