#pragma once
#include <map>

#include "pe_file.h"

namespace dlg_help_utils
{
    class pe_file_memory_mapping
    {
    public:
        pe_file_memory_mapping() = default;

        [[nodiscard]] bool is_pe_file_loaded(uint64_t address) const;
        void load_pe_file(std::wstring const& path, uint64_t module_base);
        void unload_pe_file(uint64_t module_base);

        [[nodiscard]] void const* find_address_range(uint64_t address, uint64_t length) const;
        [[nodiscard]] void const* find_any_address_range(uint64_t address, uint64_t& length) const;

    private:
        [[nodiscard]] std::map<uint64_t, pe_file>::const_iterator find_loaded_pe_file(uint64_t address) const;

    private:
        std::map<uint64_t, pe_file> loaded_pe_files_;
        std::map<uint64_t, uint64_t> loaded_module_range_;
    };
}
