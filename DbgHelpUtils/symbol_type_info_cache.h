#pragma once
#include <unordered_map>

#include "symbol_type_info.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info_cache
    {
    public:
        void create_cached_symbol_type_info(HANDLE process, DWORD64 module_base, ULONG type_index);
        [[nodiscard]] std::optional<symbol_type_info> get_symbol_type_info(ULONG type_index) const;
        [[nodiscard]] symbol_type_info get_or_create_symbol_type_info(HANDLE process, DWORD64 module_base, ULONG type_index);

    private:
        std::unordered_map<uint32_t, symbol_type_info> cache_;
    };
}
