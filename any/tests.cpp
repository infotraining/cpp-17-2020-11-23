#include <algorithm>
#include <any>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <optional>

#include "catch.hpp"

using namespace std;

TEST_CASE("any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 3;
    anything = 3.14;
    anything = "text"s;
    anything = vector {1, 2, 3};

    REQUIRE(anything.has_value());

    SECTION("any_cast")
    {
        auto vec = std::any_cast<vector<int>>(anything);
        REQUIRE(vec == vector {1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(anything), std::bad_any_cast);
    }

    SECTION("any_cast with pointer")
    {
        vector<int>* ptr_vec = std::any_cast<std::vector<int>>(&anything);

        REQUIRE(ptr_vec != nullptr);
        REQUIRE(*ptr_vec == vector {1, 2, 3});
    }

    SECTION("any + RTTI")
    {
        const type_info& type_desc = anything.type();

        std::cout << type_desc.name() << "\n";
    }

    anything.reset();

    REQUIRE(anything.has_value() == false);
}

class KeyValueDictionary
{
    std::map<std::string, std::any> dict_;

public:
    template <typename T>
    optional<map<string, any>::iterator> insert(string key, T value)
    {
        //static_assert(!is_same_v<T, std::string>, "string is not supported");

        auto [pos, was_inserted] = dict_.emplace(move(key), move(value));

        if (was_inserted)
            return pos;
        
        return nullopt;
    }

    template <typename T>
    T& at(const string& key)
    {
        T* value = any_cast<T>(&dict_.at(key));

        if (!value)
            throw bad_any_cast();

        return *value;
    }
};

TEST_CASE("KeyValueDictionary")
{
    KeyValueDictionary dict;

    dict.insert("name", "Jan"s);
    dict.insert("age", 33);
    dict.insert("data", vector{1, 2, 3});

    REQUIRE(dict.at<string>("name") == "Jan"s);
    REQUIRE(dict.at<int>("age") == 33);
}

////////////////////////////////////
// wide interfaces

class TempMonitor
{
public:
    double get_temp() const
    {
        return 23.88;
    }
};

class Observer
{
public:
    virtual void update(const std::any& sender, const string& msg) = 0;
    virtual ~Observer() = default;
};

class Logger : public Observer
{
public:
    void update(const std::any& sender, const string& msg) override
    {
        const TempMonitor* monitor = std::any_cast<TempMonitor>(&sender);
        if (monitor)
            monitor->get_temp();
    }
};