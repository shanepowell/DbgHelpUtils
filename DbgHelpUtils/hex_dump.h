#pragma once
#include <ostream>

namespace dlg_help_utils::hex_dump
{
    void hex_dump(std::wostream& os, void const* data, size_t length, size_t indent = 0, bool write_header = true,
                  size_t bytes_per_row = 0x10, uint64_t offset = 0ULL);
}
