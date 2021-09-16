#pragma once
#include <set>
#include <string>
#include <vector>

namespace dlg_help_utils::heap::statistic_views
{
    class system_module_list
    {
    public:
        system_module_list(std::vector<std::wstring> const& system_modules_names);
        system_module_list();

        [[nodiscard]] bool is_system_module(std::wstring const& module_name) const;

    private:
        std::set<std::wstring> const system_modules_names_;
    };
}
