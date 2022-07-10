#pragma once
#include <ostream>

#include "write_header.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
}

namespace dlg_help_utils::hex_dump
{
    void hex_dump(std::wostream& os, void const* data, uint64_t length, size_t indent = 0, write_header_t write_header = write_header_t{true}, uint64_t bytes_per_row = 0x10, uint64_t offset = 0ULL);
    void hex_dump(std::wostream& os, mini_dump_memory_stream& stream, uint64_t length, size_t indent = 0, write_header_t write_header = write_header_t{true}, uint64_t bytes_per_row = 0x10, uint64_t offset = 0ULL);
}
