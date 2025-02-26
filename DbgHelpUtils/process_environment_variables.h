#pragma once
#include <cstdint>

#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::process
{

    class process_environment_variables
    {
    public:
        process_environment_variables(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t process_parameters_address);

        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const { return *walker_; }
        [[nodiscard]] uint64_t process_parameters_address() const { return process_parameters_address_; }

        [[nodiscard]] std::generator<std::wstring> environment() const;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info process_parameters_symbol_info;

            stream_utils::symbol_type_and_base_type_field_offset rtl_user_process_parameters_structure_environment_field_data;
        };
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        cache_data const* cache_data_{&setup_globals()};
        stream_stack_dump::mini_dump_memory_walker const* walker_;
        uint64_t process_parameters_address_;
    };

}
