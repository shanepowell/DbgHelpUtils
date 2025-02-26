#include "process_environment_variables.h"

#include <format>

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "stream_hex_dump.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::process
{
    process_environment_variables::process_environment_variables(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const process_parameters_address)
    : cache_manager_{&cache}
    , walker_{&walker}
    , process_parameters_address_{process_parameters_address}
    {
    }

    std::generator<std::wstring> process_environment_variables::environment() const
    {
        const auto environment_address = find_field_pointer_type_and_value_in_type(walker(), cache_data_->rtl_user_process_parameters_structure_environment_field_data, process_parameters_address());
        if(!environment_address.has_value() || environment_address.value().value == 0)
        {
            co_return;
        }

        auto stream = walker().get_process_memory_stream(environment_address.value().value, std::numeric_limits<uint64_t>::max());
        if(stream.eof())
        {
            throw exceptions::wide_runtime_error{std::format(L"Can't find process Environment memory address [{0}]", stream_hex_dump::to_hex_full(environment_address.value().value))};
        }

        auto null_found = false;
        std::wstring value;

        while(!stream.eof())
        {
            wchar_t ch;
            if(stream.read(&ch, sizeof ch) != sizeof ch)
            {
                break;
            }

            if(ch == NULL)
            {
                if(null_found)
                {
                    // double null found, that's the end of the array
                    break;
                }

                null_found = true;
                if(!value.empty())
                {
                    co_yield value;
                    value.clear();
                }
                continue;
            }

            value += ch;
            null_found = false;
        }

        if(!value.empty())
        {
            co_yield value;
        }
    }

    process_environment_variables::cache_data const& process_environment_variables::setup_globals() const
    {
        if(!cache_manager_->has_cache<cache_data>())
        {
            auto& data = cache_manager_->get_cache<cache_data>();
            data.process_parameters_symbol_info = stream_utils::get_type(walker(), common_symbol_names::rtl_user_process_parameters_structure_symbol_name);
            data.rtl_user_process_parameters_structure_environment_field_data = stream_utils::get_field_type_and_offset_in_type(data.process_parameters_symbol_info, common_symbol_names::rtl_user_process_parameters_structure_symbol_name,common_symbol_names::rtl_user_process_parameters_structure_environment_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache_manager_->get_cache<cache_data>();
    }
}
