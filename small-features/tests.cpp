#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

struct Person
{
    static int id_gen;

    const int id = ++id_gen;
};

using namespace std;

TEST_CASE("test")
{
    REQUIRE(1 == 1);
}