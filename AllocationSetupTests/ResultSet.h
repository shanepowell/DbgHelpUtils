#pragma once
#include <cstdint>
#include <vector>

struct Allocation
{
    uint64_t pointer;
    size_t size;
    char fill_char;
    bool allocated;
};

struct ResultSet
{
    std::vector<Allocation> first_allocations;
    std::vector<Allocation> second_allocations;
};
