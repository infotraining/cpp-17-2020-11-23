#include <iostream>
#include <string>
#include <tuple>

#include "catch.hpp"

using namespace std;

enum class Bitfields : uint32_t
{
    value
};

////////////////////////////////////////////
// tuple like protocol - Structured Bindings

// step 1
template <>
struct std::tuple_size<Bitfields>
{
    static constexpr size_t value = 4;
};

// step 2
template <size_t Index>
struct std::tuple_element<Index, std::enable_if_t<(Index < 2), Bitfields>>
{
    using type = uint8_t;
};

template <size_t Index>
struct std::tuple_element<Index, std::enable_if_t<(Index >= 2), Bitfields>>
{
    using type = int8_t;
};


// step 3
template <size_t Index>
decltype(auto) get(const Bitfields& sth)
{
    return (static_cast<uint32_t>(sth) >> ((3 - Index) * 8) & 0xFF);
}

// template <>
// decltype(auto) get<2>(const Bitfields& sth)
// {
//     return (static_cast<uint32_t>(sth) >> 8) & 0xFF;
// }
 

TEST_CASE("split integer to bytes")
{
    Bitfields value{0b00000001'11100010'00000100'01001000};

    const auto [b1, b2, b3, b4] = value;

    static_assert(is_same_v<decltype(b1), const uint8_t>);
    static_assert(is_same_v<decltype(b3), const int8_t>);

    CHECK(b1 == 0b00000001);
    CHECK(b2 == 0b11100010);
    CHECK(b3 == 0b00000100);
    CHECK(b4 == 0b01001000);
}