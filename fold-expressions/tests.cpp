#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

TEST_CASE("fold expressions")
{
    REQUIRE(BeforeCpp17::sum(1, 2, 3, 4) == 10);
    REQUIRE(BeforeCpp17::sum(1, 2, 3.14, 4) == Approx(10.14));
}