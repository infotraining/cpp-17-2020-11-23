#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <math.h>

#include <cmath>

#include "catch.hpp"

using namespace std;

struct Circle
{
    int radius;
};

struct Rectangle
{
    int width, height;
};

struct Square
{
    int size;
};

template <typename... Closures>
struct overload : Closures...
{
    using Closures::operator()...; // new C++17 feature
};

template <typename... Closures>
overload(Closures...) -> overload<Closures...>;

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = variant<Circle, Rectangle, Square>;

    vector<Shape> shapes = {Circle{1}, Square{10}, Rectangle{10, 1}};

    double total_area {};
    for(const auto& s : shapes)
    {
        total_area += std::visit(overload {
            [](const Rectangle& r) { return 1.f * r.width * r.height; },
            [](const Square& s) { return 1.0f * s.size * s.size; },
            [](const Circle& c) { return 3.14f * c.radius * c.radius;}
            }, s
        );
    }

    REQUIRE(total_area == Approx(113.14));
}