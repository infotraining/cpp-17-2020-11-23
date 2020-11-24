#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;


struct Gadget
{
    string id;
    int usage_counter = 0;

    explicit Gadget(string id)
        : id {move(id)}
    {
        cout << "Gadget(" << this->id << " - " << this << ")\n";
    }

    Gadget(const Gadget& source)
        : id {source.id}
    {
        cout << "Gadget(cc: " << this->id << " - " << this << ")\n";
    }

    Gadget(Gadget&& source) noexcept
        : id {move(source.id)}
    {
        cout << "Gadget(mv: " << this->id << " - " << this << ")\n";
    }

    ~Gadget()
    {
        cout << "~Gadget(" << this->id << " - " << this << ")\n";
    }

    void use()
    {
        ++usage_counter;
    }

    auto get_reporter()
    {
        return [this] { cout << "Report from Gadget(" << id << ")\n"; };
    }
};

TEST_CASE("test")
{
    REQUIRE(1 == 1);
}