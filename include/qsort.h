#pragma once
#include <iterator>

auto Partition(auto first, auto last, auto compare)
{
    auto pivot = std::prev(last, 1);
    auto i = first;
    for (auto j = first; j != pivot; ++j)
    {
        if (compare(*j, *pivot))
        {
            std::swap(*i++, *j);
        }
    }
    std::swap(*i, *pivot);
    return i;
}

void QuickSort(auto first, auto last, auto compare)
{
    if (std::distance(first, last) > 1)
    {
        auto bound = Partition(first, last, compare);
        QuickSort(first, bound, compare);
        QuickSort(bound + 1, last, compare);
    }
}
