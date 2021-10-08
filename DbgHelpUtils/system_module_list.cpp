#include "system_module_list.h"

#include <filesystem>

#include "vector_to_hash_set.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap::statistic_views
{
    namespace
    {
        std::set<std::wstring> create_default_system_module_list()
        {
            return {L"ntdll.dll"s, L"kernelbase.dll"s, L"kernel32.dll"s, L"msvcp140.dll"s, L"msvcp140d.dll"s, L"ucrtbase.dll"s};
        }
    }

    system_module_list::system_module_list(std::vector<std::wstring> const& system_modules_names)
    : system_modules_names_{vector_to_hash_set<wstring_to_lower>(system_modules_names)}
    {
        ;
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
}
