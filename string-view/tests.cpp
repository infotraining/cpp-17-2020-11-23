#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("string_view")
{
    SECTION("creation")
    {
        const char* ctext = "text";
        string_view sv1 = ctext;
        REQUIRE(sv1 == "text"sv);
        REQUIRE(sv1.length() == 4);

        std::string str = "str";
        string_view sv2 = str;

        const char* words = "abc def ghi";
        string_view token1(words, 3);
        REQUIRE(token1 == "abc"sv);

        string_view token2(words + 4, 3);
        REQUIRE(token2 == "def"sv);
    }

    SECTION("string_view vs. c-string & std::string")
    {
        char text[] = {'a', 'b', 'c'};

        string_view sv(text, 3);

        std::cout << sv << "\n";
    }
}

template <typename Container>
void print_all(const Container& container, string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("print all")
{
    vector vec = {1, 2, 3};

    print_all(vec, "vec");

    string prefix = "vec";
    print_all(vec, prefix);

    print_all(vec, "vec"s);
}

string_view get_prefix(string_view text, size_t length)
{
    return {text.data(), length};
}

namespace Error
{
    // void open_file(string_view filename)
    // {
    //     fopen(filename.data(), "+w");
    // }
}

TEST_CASE("beware - danger")
{
    const char* text = "abcdef";

    string_view pf1 = get_prefix(text, 3);
    REQUIRE(pf1 == "abc"sv);

    SECTION("dangling pointer")
    {
        string_view pf2 = get_prefix("absded"s, 3);
        //REQUIRE(pf2 == "abc"sv);
    }

    std::string str3 = "test";
    std::string_view sv = "TEST";

    std::string str4 = "start";
    str4 += std::string(sv);
}

TEST_CASE("conversions")
{
    string str = "abc";
    string_view sv1 = str;

    string str2 = std::string(sv1);
}

TEST_CASE("default constructor")
{
    string_view sv;
    REQUIRE(sv.data() == nullptr);

    string s;
    REQUIRE(s.data() != nullptr);
}