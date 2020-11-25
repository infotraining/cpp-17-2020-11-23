#include <algorithm>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <variant>
#include <vector>

#include "catch.hpp"

using namespace std;

struct NoDefaultConstructible
{
    int value;

    NoDefaultConstructible(int v)
        : value {v}
    {
    }
};

TEST_CASE("variant")
{
    std::variant<int, double, std::string, std::vector<int>> v1;

    REQUIRE(std::holds_alternative<int>(v1) == true);
    REQUIRE(std::get<int>(v1) == 0);

    std::variant<std::monostate, NoDefaultConstructible, int, double, std::string, std::vector<int>> v2;
    REQUIRE(v2.index() == 0);

    v1 = 3.14;
    v1 = "text"s;
    v1 = vector {1, 2, 3};

    REQUIRE(std::get<vector<int>>(v1) == vector {1, 2, 3});
    REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);

    REQUIRE(std::get_if<int>(&v1) == nullptr);
    REQUIRE(std::get_if<std::vector<int>>(&v1) != nullptr);

    if (auto ptr = std::get_if<std::vector<int>>(&v1); ptr)
        ptr->resize(42);
}

struct Value
{
    float v;

    Value(float v)
        : v {v}
    {
    }

    ~Value()
    {
        cout << "~Value(" << v << ")\n";
    }
};

struct Evil
{
    string v;

    Evil(string v)
        : v {std::move(v)}
    {
    }

    Evil(Evil&& other)
    {
        throw std::runtime_error("42");
    }
};

TEST_CASE("valueless variant")
{
    variant<Value, Evil> v {12.0f};

    try
    {
        v.emplace<Evil>(Evil {"evil"});
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    REQUIRE(v.valueless_by_exception());
    REQUIRE(v.index() == std::variant_npos);
}

struct Printer
{
    void operator()(int x) const
    {
        std::cout << "INT: " << x << "\n";
    }

    void operator()(double x) const
    {
        std::cout << "DOUBLE: " << x << "\n";
    }

    void operator()(const string& x) const
    {
        std::cout << "STRING: " << x << "\n";
    }
};

template <typename... Closures>
struct overload : Closures...
{
    using Closures::operator()...; // new C++17 feature
};

template <typename... Closures>
overload(Closures...) -> overload<Closures...>;

TEST_CASE("visiting variants")
{
    std::variant<int, short, double, std::string> v = "abc"s;
    std::visit(Printer {}, v);

    auto printer = [](const auto& item) { std::cout << typeid(item).name() << " - " << item << "\n"; };
    std::visit(printer, v);

    int result{};

    auto better_printer = overload {
        [&](int x) { std::cout << "INT: " << x << "\n"; result += x; },
        [&](double x) { std::cout << "DOUBLE: " << x << "\n"; result += static_cast<int>(x);},
        [&](const string& s) { std::cout << "STRING: " << s << "\n"; result += s.size(); }
    };

    std::visit(better_printer, v);

    REQUIRE(result == 3);
}