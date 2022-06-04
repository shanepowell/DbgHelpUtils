#pragma once
#include <set>
#include <string>
#include <vector>

namespace dlg_help_utils
{
    class unloaded_module_list_stream;
    class module_list_stream;
}

namespace dlg_help_utils::heap
{
    class system_module_list
    {
    public:
        explicit system_module_list(std::vector<std::wstring> const& system_modules_names);
        system_module_list();

        [[nodiscard]] bool is_system_module(std::wstring const& module_name) const;
        [[nodiscard]] std::set<uint64_t> generate_system_module_bases(module_list_stream const& module_list, unloaded_module_list_stream const& unloaded_module_list) const;

    private:
        std::set<std::wstring> system_modules_names_;
    };
}
