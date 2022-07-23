﻿#include "pe_file_memory_mapping.h"

#include <format>

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
        if (auto const it = find_loaded_pe_file(module_base); it != loaded_pe_files_.end())
        {
            throw wide_runtime_error{std::format(L"Error: module {0} base : {1} already used by {2}", path, stream_hex_dump::to_hex(module_base), it->second.path())};
        }

        pe_file file{path};
        file.open_pe();

        if (!file.is_valid() || (!file.is_x86_pe() && !file.is_x64_pe()))
        {
            throw wide_runtime_error{std::format(L"Error: module {} not a valid pe file", path)};
        }

        loaded_module_range_.insert(std::make_pair(module_base + file.file_length() - 1, module_base));
        loaded_pe_files_.insert(std::make_pair(module_base, std::move(file)));
    }

    void pe_file_memory_mapping::unload_pe_file(uint64_t const module_base)
    {
        
        if(auto const it = loaded_pe_files_.find(module_base); it != loaded_pe_files_.end())
        {
            loaded_module_range_.erase(module_base + it->second.file_length() - 1);
            loaded_pe_files_.erase(it);
        }
    }

    void const* pe_file_memory_mapping::find_address_range(uint64_t const address, uint64_t const length) const
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

    void const* pe_file_memory_mapping::find_any_address_range(const uint64_t address, uint64_t& length) const
    {
        if(length > 0)
        {
            range_utils::limit_range(address, length);
            if (length == 0) return nullptr;
        }

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

    std::map<uint64_t, pe_file>::const_iterator pe_file_memory_mapping::find_loaded_pe_file(uint64_t const address) const
    {
        if (loaded_pe_files_.empty()) return loaded_pe_files_.end();

        const auto it = loaded_module_range_.lower_bound(address);
        if(it == loaded_module_range_.end()) return loaded_pe_files_.end();
        if (address < it->second || address > it->first) return loaded_pe_files_.end();
        return loaded_pe_files_.find(it->second);
    }
}
