#include "dump_file_options.h"

#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;

namespace
{
    std::vector<std::wstring> const g_empty_values;
}

dump_file_options::dump_file_options(boost::program_options::variables_map const& vm)
    : dump_header_{ vm.count("header") > 0 }
    , dump_streams_{ vm.count("streams") > 0 }
    , generate_crc32_{ vm.count("crc32") > 0 }
    , hex_dump_stream_data_{ vm.count("hexdump") > 0 }
    , hex_dump_memory_data_{ vm.count("memoryhexdump") > 0 }
    , continue_on_errors_{ vm.count("continue") > 0 }
    , display_symbols_{ vm.count("symbols") > 0 }
    , debug_symbols_{ vm.count("symboldebug") > 0 || vm.count("symboldebugmemory") > 0 }
    , debug_load_symbols_memory_{ vm.count("symboldebugmemory") > 0 }
    , debug_type_data_{ vm.count("typedebug") > 0 }
    , display_peb_{ vm.count("peb") > 0 }
    , display_heap_{ vm.count("heap") > 0 }
    , display_heap_entries_{ vm.count("heapentries") > 0 }
    , display_crtheap_{ vm.count("crtheap") > 0 }
    , debug_heap_data_{ vm.count("heapdebug") > 0 }
    , display_stack_trace_database_{ vm.count("std") > 0 }
{
    if (vm.count("streamindex") > 0)
    {
        dump_stream_indexes_ = vm["streamindex"].as<vector<size_t>>();
    }

    if (vm.count("streamtype") > 0)
    {
        for (auto stream_types = vm["streamtype"].as<vector<wstring>>(); auto const& stream_type : stream_types)
        {
            dump_stream_types_.emplace_back(dlg_help_utils::mini_dump_stream_type::from_string(stream_type));
        }
    }

    if (vm.count("filter") > 0)
    {
        for (auto filter_values = vm["filter"].as<vector<wstring>>(); auto const& filter_value : filter_values)
        {
            auto const pos = filter_value.find_first_of('=');
            if (pos == std::wstring::npos)
            {
                std::wostringstream ss;
                ss << "Filter value: [" << filter_value << "] format incorrect. Expecting <name>=<value> format";
                throw dlg_help_utils::exceptions::wide_runtime_error{std::move(ss).str()};
            }

            auto name = filter_value.substr(0, pos);
            auto value = filter_value.substr(pos + 1);

            if (name.empty() || value.empty())
            {
                std::wostringstream ss;
                ss << "Filter value: [" << filter_value << "] format incorrect. Expecting <name>=<value> format";
                throw dlg_help_utils::exceptions::wide_runtime_error{std::move(ss).str()};
            }

            filter_values_[name].emplace_back(std::move(value));
        }
    }

    if(vm.count("limitmemoryhexdump") > 0)
    {
        limit_hex_dump_memory_size_ = dlg_help_utils::size_units::base_10::from_wstring(vm["limitmemoryhexdump"].as<wstring>()).count();
    }

    if(vm.count("type"))
    {
        symbol_types_ = vm["type"].as<vector<wstring>>();
    }

    if(vm.count("moduletypes"))
    {
        dump_types_modules_ = vm["moduletypes"].as<vector<wstring>>();
    }

    if(vm.count("address"))
    {
        dump_address_types_ = vm["address"].as<vector<wstring>>();
    }

    if(vm.count("symbol"))
    {
        symbol_names_ = vm["symbol"].as<vector<wstring>>();
    }
}

size_t dump_file_options::hex_dump_memory_size(size_t const size) const
{
    if(limit_hex_dump_memory_size() > 0)
    {
        return std::min(limit_hex_dump_memory_size(), size);
    }
    return size;
}

std::vector<std::wstring> const& dump_file_options::filter_values(std::wstring const& option) const
{
    if (auto const it = filter_values_.find(option); it != filter_values_.end())
    {
        return it->second;
    }

    return g_empty_values;
}
