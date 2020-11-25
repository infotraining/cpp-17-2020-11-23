#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

using namespace std;

std::vector<std::string_view> split_text(std::string_view text, std::string_view sep = " ")
{
    std::vector<std::string_view> result;
    while(true)
    {
        auto pos = text.find(sep);
        if(pos != text.npos)
        {
            result.emplace_back(text.data(), pos);
            text = { text.data() + pos + sep.size(), text.size() - pos - sep.size() };
        }
        else
        {
            result.push_back(text);
            break;
        }
    }
    return result;
}

std::vector<std::string> split_string(std::string_view text, std::string_view sep = " ")
{
    std::vector<std::string> result;
    while(true)
    {
        auto pos = text.find(sep);
        if(pos != text.npos)
        {
            result.emplace_back(text.data(), pos);
            text = { text.data() + pos + sep.size(), text.size() - pos - sep.size() };
        }
        else
        {
            result.push_back(string(text));
            break;
        }
    }
    return result;
}

TEST_CASE("split with spaces")
{
    string text = "one two three four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    text = "onesdfsdfsdfsdfsdfsdfsdfsdfsdf twosdffsdfsdfsdgdfhdfghfgh threegdfgdfhfghfghfghfghfghfg fourfghfghfghfghfghfghfghfghfg fghfgjhgjhfgjfgbvcnghjfgjhfhfgjjjhfgjhhfgjfgjfg";

    BENCHMARK("split with string_view")
    {
        return split_text(text);
    };

    BENCHMARK("split with strings")
    {
        return split_string(text);
    };    
}

TEST_CASE("split with commas")
{
    string text = "one,two,three,four";

    auto words = split_text(text, ",");

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(words), end(words), begin(expected)));
}
