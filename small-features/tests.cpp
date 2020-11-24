#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

// hpp
struct Person
{
    inline static int id_gen = 0; // definition

    const int id = ++id_gen;
};

using namespace std;

TEST_CASE("test")
{
    Person p1;
    REQUIRE(p1.id == 1);

    Person p2;
    REQUIRE(p2.id == 2);
}

template <typename TContainer>
void print(string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

struct Aggregate1
{
    int a;
    double b;
    int coord[3];
    std::string name;

    void print() const
    {
        std::cout << "Aggregate1{" << a << ", " << b;
        std::cout << ", [ ";
        for (const auto& item : coord)
            std::cout << item << " ";
        std::cout << "], '" << name << "'}\n";
    }
};

// since C++17
struct Aggregate2 : std::string, Aggregate1
{
    std::vector<int> data;
};

TEST_CASE("aggregate")
{
    int tab1[10];
    print("tab1", tab1);

    int tab2[10] = {};
    print("tab2", tab2);

    int tab3[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print("tab3", tab3);

    int tab4[10] = {1, 2, 3};
    print("tab4", tab4);

    static_assert(std::is_aggregate_v<Aggregate1>);

    Aggregate1 agg1;
    agg1.print();

    Aggregate1 agg2 {1, 3.14, {1, 2}, "abc"};
    agg2.print();

    Aggregate1 agg3 {};
    agg3.print();

    Aggregate2 object {{"abc"}, {1, 3.14, {1}, "agg1"}, {6, 7, 8}};

    REQUIRE(object.size() == 3);
    object.print();
    REQUIRE(object.data == vector {6, 7, 8});
}

struct ErrorCode
{
    int err_code;
    const char* msg;
};

void step1() { }
void step2() { }
void step3() { }

namespace [[deprecated]] LegacyCode
{
    void f(int n)
    {
        switch (n)
        {
        case 1:
        case 2:
            step1();
            [[fallthrough]];
        case 3: // no warning on fallthrough
            step2();
            break;
        case 4: // compiler may warn on fallthrough
            step3();
        }
    }
}

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    [[maybe_unused]] int flag = 42;

    return ErrorCode {13, "Error#13"};
}

TEST_CASE("attributes")
{
    auto result = open_file("text.txt");
    REQUIRE(result.err_code == 13);

    LegacyCode::f();
}