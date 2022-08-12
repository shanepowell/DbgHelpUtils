#include "symbol_type_info_cache.h"

namespace dlg_help_utils::dbg_help
{
    void symbol_type_info_cache::create_cached_symbol_type_info(HANDLE const process, DWORD64 const module_base, ULONG const type_index, std::wstring_view const export_name)
    {
        std::ignore = get_or_create_symbol_type_info(process, module_base, type_index, export_name);
    }

    std::optional<symbol_type_info> symbol_type_info_cache::get_symbol_type_info(HANDLE const process, DWORD64 const module_base, ULONG const type_index) const
    {
        auto const it = cache_.find(symbol_type_info{symbol_type_info::key_compare_only{}, process, module_base, type_index});
        if(it == cache_.end())
        {
            return std::nullopt;
        }

        return *it;
    }

    symbol_type_info symbol_type_info_cache::get_or_create_symbol_type_info(HANDLE const process, DWORD64 const module_base, ULONG const type_index, std::wstring_view const export_name)
    {
        return *cache_.insert(symbol_type_info{*this, process, module_base, type_index, export_name}).first;
    }
}
