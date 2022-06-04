﻿#include "system_module_list.h"

#include <filesystem>

#include "module_list_stream.h"
#include "unloaded_module_list_stream.h"
#include "vector_to_hash_set.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    namespace
    {
        std::set<std::wstring> create_default_system_module_list()
        {
            return {L"ntdll.dll"s, L"kernelbase.dll"s, L"kernel32.dll"s, L"msvcp140.dll"s, L"msvcp140d.dll"s, L"ucrtbase.dll"s, L"ucrtbased.dll"s, L"vcruntime140.dll"s, L"vcruntime140d.dll"s, L"vcruntime140_1.dll"s, L"vcruntime140_1d.dll"s, L"sechost.dll"s};
        }
    }

    system_module_list::system_module_list(std::vector<std::wstring> const& system_modules_names)
    : system_modules_names_{vector_to_hash_set<wstring_to_lower>(system_modules_names)}
    {
    }

    system_module_list::system_module_list()
    : system_modules_names_{create_default_system_module_list()}
    {
    }

    bool system_module_list::is_system_module(std::wstring const& module_name) const
    {
        auto const key = string_compare::to_lower(std::filesystem::path{module_name}.filename().wstring());
        return system_modules_names_.contains(key);
    }

    std::set<uint64_t> system_module_list::generate_system_module_bases(module_list_stream const& module_list, unloaded_module_list_stream const& unloaded_module_list) const
    {
        std::set<uint64_t> bases;

        for (auto const& system_modules_name : system_modules_names_)
        {
            if(auto const* module = module_list.find_module(system_modules_name); module)
            {
                bases.insert((*module)->BaseOfImage);
            }
            else if(auto const* unloaded_module = unloaded_module_list.find_module(system_modules_name); unloaded_module)
            {
                bases.insert((*unloaded_module)->BaseOfImage);
            }
        }

        return bases;
    }
}
