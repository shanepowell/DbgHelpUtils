#pragma once
#include <optional>

namespace dlg_help_utils
{
    class memory64_list_stream;
    class memory_list_stream;
    class thread_names_list_stream;
    class mini_dump;
}

namespace dlg_help_utils::common_symbol_utils
{

    std::optional<uint64_t> get_teb_address(mini_dump const& mini_dump, thread_names_list_stream const& names_list, memory_list_stream const& memory_list, memory64_list_stream const& memory64_list);

}
