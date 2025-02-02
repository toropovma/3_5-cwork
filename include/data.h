#pragma once
#include <fmt/base.h>
#include <fmt/ostream.h>

#include <vector>

struct Data
{
    std::vector<int> original;
    std::vector<int> sorted;
};

template<typename T>
std::ostream& operator<<(std::ostream& ostream, const std::vector<T>& vec)
{
    bool first = true;
    for (const auto& value : vec)
    {
        if (!first) ostream << ", ";
        first = false;
        ostream << value;
    }
    return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const Data& data);

template<>
struct fmt::formatter<Data> : public fmt::ostream_formatter {};
