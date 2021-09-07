#pragma once
#include <cstdint>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4267)
#include <json_struct/include/json_struct.h>
#pragma warning(pop)

struct Allocation
{
    uint64_t pointer;
    size_t size;
    char fill_char;
    bool allocated;

    JS_OBJ(pointer, size, fill_char, allocated);
};

struct ResultSet
{
    std::vector<Allocation> allocations;

    JS_OBJ(allocations);
};
