#include "process_parameters.h"

#include <sstream>

#include "common_symbol_names.h"
#include "stream_hex_dump.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::process
{
    process_parameters::process_parameters(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const process_parameters_address)
    : walker_{walker}
    , process_parameters_address_{process_parameters_address}
    , process_parameters_symbol_info_{stream_utils::get_type(walker_, common_symbol_names::rtl_user_process_parameters_structure_symbol_name)}
    {
    }

    std::experimental::generator<std::wstring_view> process_parameters::environment() const
    {
        const auto environment_address = stream_utils::find_field_pointer_type_and_value_in_type(walker(), process_parameters_symbol_info_, common_symbol_names::rtl_user_process_parameters_structure_environment_field_symbol_name, process_parameters_address());
        if(!environment_address.has_value() || environment_address.value().second == 0)
        {
            co_return;
        }

        DWORD64 environment_size = std::numeric_limits<uint64_t>::max();
        auto const* environment_variable = walker().get_process_memory_range(environment_address.value().second, environment_size);
        if(environment_variable == nullptr)
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} << "Can't find process Environment memory address [" << stream_hex_dump::to_hex_full(environment_address.value().second) << "]").str()};
        }

        auto null_found = false;
        auto const* start = static_cast<wchar_t const*>(environment_variable);
        auto const* str = start;

        for(DWORD64 i = 0; i < environment_size; ++i, ++str)
        {
            if(*str == NULL)
            {
                if(null_found)
                {
                    // double null found, that's the end of the array
                    co_return;
                }

                null_found = true;
                co_yield std::wstring_view{start, static_cast<size_t>(str - start)};
                start = str + 1;
                continue;
            }

            null_found = false;
        }
    }
}
