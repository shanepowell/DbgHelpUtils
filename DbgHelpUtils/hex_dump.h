#pragma once
#include <ostream>

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
}

namespace dlg_help_utils::hex_dump
{
    void hex_dump(std::wostream& os, void const* data, uint64_t length, size_t indent = 0, bool write_header = true, uint64_t bytes_per_row = 0x10, uint64_t offset = 0ULL);
    void hex_dump(std::wostream& os, mini_dump_memory_stream& stream, uint64_t length, size_t indent = 0, bool write_header = true, uint64_t bytes_per_row = 0x10, uint64_t offset = 0ULL);
}
