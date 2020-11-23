#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <mutex>
#include <queue>

#include "catch.hpp"

using namespace std;

TEST_CASE("if/switch with initializer")
{
    std::vector data = {1, 42, 665, 3453, 6576};

    if (auto pos = std::find(begin(data), end(data), 665); pos != end(data))
    {
        std::cout << "Item " << *pos << " has been found\n";
    }
    else
    {
        std::cout << "Item has not been found\n";

        assert(pos == end(data));
    }

    SECTION("with structured bindings")
    {
        std::map<int, string> dict;

        if (auto [pos, is_inserted] = dict.insert(std::pair(42, "fourty two"s)); !is_inserted)
        {
            const auto& [key, value] = *pos;

            cout << key << " is already in a dictionary\n";
        }
    }
}

TEST_CASE("if with mutex")
{
    queue<int> q;
    mutex mtx_q;

    {
        lock_guard lk{mtx_q};
        q.push(42);
    }

    SECTION("pop")
    {        
        if (lock_guard lk{mtx_q}; !q.empty())
        {
            auto value = q.front();
            q.pop();
        } // unlock na mutexie        
    }    
}

////////////////////////////////////////////////////
// constexpr if

template <typename T1, typename T2, size_t N, size_t M>
auto my_copy(T1(&source)[N], T2(&target)[M])
{
    static_assert(M >= N, "size of target array must be larger than source");

    if constexpr(std::is_same_v<T1, T2> && std::is_trivially_copyable_v<T1>)
    {
        memcpy(target, source, N * sizeof(T1));
        std::cout << "my_copy by memcpy\n";

    }
    else
    {
        for(size_t i = 0; i < N; ++i)
            target[i] = source[i];
        std::cout << "my_copy by loop\n";
    }   
}


TEST_CASE("mcopy")
{
    std::string words1[] = { "one", "two", "three"};
    std::string words2[3];

    my_copy(words1, words2);

    REQUIRE(std::equal(begin(words2), end(words2), begin(words1)));    

    int tab1[] = {1, 2, 3};
    int tab2[3];

    my_copy(tab1, tab2);

    REQUIRE(std::equal(begin(tab1), end(tab1), begin(tab2)));    
}

// void print()
// {
//     std::cout << "\n";
// }

template <typename Head, typename... Tail>
void print(const Head& h, const Tail&... tail)
{
    std::cout << h << " ";
    
    if constexpr(sizeof...(tail) > 0)
        print(tail...);
    else
        std::cout << "\n";
}

template <typename... Ts>
struct get_size
{
    static constexpr size_t value = sizeof...(Ts);
};

TEST_CASE("variadic templates & constexpr if")
{
    print(1, 3.14, "text"s);

    static_assert(get_size<int, double, string>::value == 3);
}