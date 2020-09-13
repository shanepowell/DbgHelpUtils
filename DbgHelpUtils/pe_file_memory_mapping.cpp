#include "pe_file_memory_mapping.h"

#include <sstream>


#include "range_units.h"
#include "stream_hex_dump.h"
#include "wide_runtime_error.h"

using namespace std;
using namespace dlg_help_utils::exceptions;

namespace dlg_help_utils
{
    bool pe_file_memory_mapping::is_pe_file_loaded(uint64_t const address) const
    {
        return find_loaded_pe_file(address) != loaded_pe_files_.end();
    }

    void pe_file_memory_mapping::load_pe_file(std::wstring const& path, uint64_t const module_base)
    {
        auto const it = find_loaded_pe_file(module_base);
        if (it != loaded_pe_files_.end())
        {
            throw wide_runtime_error{
                (wostringstream{} << L"Error: module " << path << L" base : " << stream_hex_dump::to_hex(module_base) <<
                    L" already used by " << it->second.path()).str()
            };
        }

        pe_file file{path};
        file.open_pe();

        if (!file.is_valid() || (!file.is_x86_pe() && !file.is_x64_pe()))
        {
            throw wide_runtime_error{(wostringstream{} << L"Error: module " << path << L" not a valid pe file").str()};
        }

        loaded_pe_files_.insert(std::make_pair(module_base, std::move(file)));
    }

    void pe_file_memory_mapping::unload_pe_file(uint64_t const module_base)
    {
        loaded_pe_files_.erase(module_base);
    }

    void const* pe_file_memory_mapping::find_address_range(uint64_t const address, size_t const length) const
    {
        if (!range_utils::validate_range(address, length)) return nullptr;

        auto const it = find_loaded_pe_file(address);
        if (it == loaded_pe_files_.end())
        {
            return nullptr;
        }

        if (!range_utils::range_contains(it->first, it->second.file_length(), address, length))
        {
            return nullptr;
        }
        return static_cast<uint8_t const*>(it->second.data()) + (address - it->first);
    }

    void const* pe_file_memory_mapping::find_any_address_range(const uint64_t address, size_t& length) const
    {
        range_utils::limit_range(address, length);
        if (length == 0) return nullptr;

        auto const it = find_loaded_pe_file(address);
        if (it == loaded_pe_files_.end())
        {
            return nullptr;
        }

        if (!range_utils::range_union(it->first, it->second.file_length(), address, length))
        {
            return nullptr;
        }
        return static_cast<uint8_t const*>(it->second.data()) + (address - it->first);
    }

    std::map<uint64_t, pe_file>::const_iterator pe_file_memory_mapping::find_loaded_pe_file(
        uint64_t const address) const
    {
        if (loaded_pe_files_.empty()) return loaded_pe_files_.end();

        auto it = loaded_pe_files_.lower_bound(address);
        if (it != loaded_pe_files_.end() && it->first == address) return it;
        if (it == loaded_pe_files_.begin()) return loaded_pe_files_.end();
        --it;
        auto const end_range = it->first + it->second.file_length();
        if (address < it->first || address >= end_range) return loaded_pe_files_.end();
        return it;
    }
}
