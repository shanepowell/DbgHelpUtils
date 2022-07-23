﻿#include "symbol_type_info_cache.h"

namespace dlg_help_utils::dbg_help
{
    void symbol_type_info_cache::create_cached_symbol_type_info(HANDLE const process, DWORD64 const module_base, ULONG type_index)
    {
        if(cache_.contains(type_index))
        {
            return;
        }

        cache_.insert(std::make_pair(type_index, symbol_type_info{*this, process, module_base, type_index}));
    }

    std::optional<symbol_type_info> symbol_type_info_cache::get_symbol_type_info(ULONG const type_index) const
    {
        auto const it = cache_.find(type_index);
        if(it == cache_.end())
        {
            return std::nullopt;
        }

        return it->second;
    }

    symbol_type_info symbol_type_info_cache::get_or_create_symbol_type_info(HANDLE const process, DWORD64 const module_base, ULONG type_index)
    {
        if(auto const it = cache_.find(type_index);
            it != cache_.end())
        {
            return it->second;
        }

        return cache_.insert(std::make_pair(type_index, symbol_type_info{*this, process, module_base, type_index})).first->second;
    }
}