#include <iostream>
#include <string>
#include <tuple>

#include "catch.hpp"

using namespace std;

enum class Bitfields : uint32_t
{
};

// step 1
template <>
struct std::tuple_size<Bitfields> : std::integral_constant<size_t, 4>
{    
};


// step 2
template <size_t Index>
struct std::tuple_element<Index, Bitfields> : std::common_type<uint8_t>
{
};

// step 3
template <size_t>
decltype(auto) get(const Bitfields&);

template <size_t Index>
decltype(auto) get(const Bitfields& sth)
{
    return (uint32_t(sth) >> ((3 - Index) * 8)) & 0xFF;
    //return (*(uint8_t (*)[4]) &sth)[3 - Index];        
}

TEST_CASE("split integer to bytes")
{
    Bitfields value{0b00000001'11100010'00000100'01001000};
    //auto [b4, b3, b2, b1] = *(uint8_t (*)[4]) &value;
    auto [b1, b2, b3, b4] = value;

    CHECK(b1 == 0b00000001);
    CHECK(b2 == 0b11100010);
    CHECK(b3 == 0b00000100);
    CHECK(b4 == 0b01001000);
}

struct Data
{
    int x;
    int& ref_x;
};

TEST_CASE("broken deduction")
{
    int x = 10;
    int& ref_x = x;
    
    Data data {x, ref_x};

    auto [ax1, ax2] = data;

    static_assert(is_same_v<decltype(ax1), int>);
    static_assert(is_same_v<decltype(ax2), int&>);
}