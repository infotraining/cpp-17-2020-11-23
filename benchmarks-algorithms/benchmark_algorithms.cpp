#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <execution>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>

std::vector<std::string> load_words(const std::string& file_name)
{
    std::ifstream input_file{file_name};

    if (!input_file)
        throw std::runtime_error("IO Error - file not found");

    std::vector<std::string> words;

    std::string token;
    while (input_file >> token)
    {
        words.push_back(token);
    }

    return words;
}

inline const std::vector<std::string> words = [] { auto words = load_words("tokens.txt"); words.resize(words.size() / 10);  return words; }();

TEST_CASE("accumulate")
{
    std::cout << "No of cores: " << std::thread::hardware_concurrency() << "\n";
    std::cout << "No of words: " << words.size() << std::endl;

    BENCHMARK("std::accumulate")
    {
        return std::accumulate(words.begin(), words.end(), 0ULL, [](const auto& total, const auto& word) { return total + std::hash<std::string>{}(word); });
    };

    BENCHMARK("std::transform_reduce - parallel")
    {
        return std::transform_reduce(std::execution::par, words.begin(), words.end(), 0ULL, std::plus{}, [](const auto& word) { return std::hash<std::string>{}(word); });
    };

    BENCHMARK("std::transform_reduce - parallel unsequenced")
    {
        return std::transform_reduce(std::execution::par_unseq, words.begin(), words.end(), 0ULL, std::plus{}, [](const auto& word) { return std::hash<std::string>{}(word); });
    };
}

TEST_CASE("sort")
{
    SECTION("sequenced")
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        BENCHMARK("std::sort")
        {
            std::sort(words_to_sort.begin(), words_to_sort.end(), [](const auto& a, const auto& b) { return boost::to_lower_copy(a) < boost::to_lower_copy(b); });
            return words_to_sort.front();
        };
    }

    SECTION("parallel")
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        BENCHMARK("std::sort - parallel")
        {
            std::sort(
                std::execution::par,
                words_to_sort.begin(), words_to_sort.end(),
                [](const auto& a, const auto& b) { return boost::to_lower_copy(a) < boost::to_lower_copy(b); });

            return words_to_sort.front();
        };
    }

    SECTION("parallel unsequenced")
    {
        auto words_to_sort = words;
        REQUIRE_FALSE(std::is_sorted(words_to_sort.begin(), words_to_sort.end()));

        BENCHMARK("std::sort - parallel unseqenced")
        {
            std::for_each(std::execution::par, words_to_sort.begin(), words_to_sort.end(), [](auto& w) { boost::to_lower(w); });
            std::vector<std::string_view> words_views(words_to_sort.size());
            std::transform(std::execution::par, words_to_sort.begin(), words_to_sort.end(), words_views.begin(), [](const auto& w) { return std::string_view(w); });

            std::sort(
                std::execution::par_unseq,
                words_views.begin(), words_views.end());

            return std::string(words_views.front());
        };
    }
}

bool is_prime(uint64_t number)
{
    if (number < 2)
    {
        return false;
    }
    else if (number % 2 == 0 && number != 2)
    {
        return false;
    }
    else
    {
        for (uint64_t i = 3; i <= sqrt(number); i += 2)
        {
            if (number % i == 0)
                return false;
        }
        return true;
    }
}

const size_t no_of_items = 20'000;

const std::vector<uint64_t> numbers = [] {
    std::random_device rd;
    std::mt19937_64 rnd_gen{rd()};
    std::uniform_int_distribution<uint64_t> rnd_distr(0, no_of_items);

    std::vector<uint64_t> numbers(no_of_items);
    std::generate(numbers.begin(), numbers.end(), [&] { return rnd_distr(rnd_gen); });

    return numbers;
}();

TEST_CASE("transform")
{
    SECTION("sequenced")
    {
        auto numbers_to_part = numbers;
        decltype(numbers_to_part) are_primes(numbers_to_part.size());

        BENCHMARK("transform - sequenced")
        {
            std::transform(numbers_to_part.begin(), numbers_to_part.end(), are_primes.begin(), [](auto n) { return is_prime(n); });
            return are_primes;
        };
    }

    SECTION("parallel")
    {
        auto numbers_to_part = numbers;
        decltype(numbers_to_part) are_primes(numbers_to_part.size());


        BENCHMARK("transform - parallel")
        {
            std::transform(std::execution::par_unseq, numbers_to_part.begin(), numbers_to_part.end(), are_primes.begin(), [](auto n) { return is_prime(n); });
            return are_primes;
        };
    }
}

TEST_CASE("partition")
{
    SECTION("sequenced")
    {
        auto numbers_to_part = numbers;

        BENCHMARK("partition - sequenced")
        {
            return std::partition(numbers_to_part.begin(), numbers_to_part.end(), [](auto n) { return is_prime(n); });
        };
    }

    SECTION("parallel")
    {
        auto numbers_to_part = numbers;

        BENCHMARK("partition - parallel")
        {
            return std::partition(std::execution::par_unseq, numbers_to_part.begin(), numbers_to_part.end(), [](auto n) { return is_prime(n); });
        };
    }
}