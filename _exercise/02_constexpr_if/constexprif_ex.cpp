#include <iostream>
#include <list>
#include <vector>

#include "catch.hpp"

using namespace std;

template <typename Iterator>
auto advance_it(Iterator& it, size_t n)
{
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
    
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, iterator_category>)
    {
        it += n;
        
        return iterator_category{};
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            it++;
        }

        return iterator_category{};
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        vector<int> target(data.size());
        
        auto it = data.begin();

        auto result = advance_it(it, 3);
        static_assert(is_same_v<decltype(result), std::random_access_iterator_tag>);

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        auto result = advance_it(it, 3);
        static_assert(!is_same_v<decltype(result), std::random_access_iterator_tag>);

        REQUIRE(*it == 4);
    }
}