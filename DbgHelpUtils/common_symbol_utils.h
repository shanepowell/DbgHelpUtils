#pragma once
#include <set>
#include <vector>

namespace dlg_help_utils
{
    class mini_dump;
}

namespace dlg_help_utils::common_symbol_utils
{

    std::vector<uint64_t> get_teb_addresses(mini_dump const& mini_dump);
    void gather_system_addresses(mini_dump const& mini_dump, std::set<uint64_t>& system_area_addresses);

}
