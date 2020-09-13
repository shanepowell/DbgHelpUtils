﻿#include "dump_file_options.h"

#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;

namespace
{
    std::vector<std::wstring> const g_empty_values;
}

dump_file_options::dump_file_options(boost::program_options::variables_map const& vm)
    : dump_header_{vm.count("header") > 0}
      , dump_streams_{vm.count("streams") > 0}
      , generate_crc32_{vm.count("crc32") > 0}
      , hex_dump_stream_data_{vm.count("hexdump") > 0}
      , hex_dump_memory_data_{vm.count("memoryhexdump") > 0}
      , continue_on_errors_{vm.count("continue") > 0}
      , load_symbols_{vm.count("symbols") > 0}
      , debug_load_symbols_{vm.count("symboldebug") > 0 || vm.count("symboldebugmemory") > 0}
      , debug_load_symbols_memory_{vm.count("symboldebugmemory") > 0}
{
    if (vm.count("streamindex") > 0)
    {
        dump_stream_indexes_ = vm["streamindex"].as<vector<size_t>>();
    }

    if (vm.count("streamtype") > 0)
    {
        auto stream_types = vm["streamtype"].as<vector<wstring>>();

        for (auto const& stream_type : stream_types)
        {
            dump_stream_types_.emplace_back(dlg_help_utils::mini_dump_stream_type::from_string(stream_type));
        }
    }

    if (vm.count("filter") > 0)
    {
        auto filter_values = vm["filter"].as<vector<wstring>>();
        for (auto const& filter_value : filter_values)
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
}

std::vector<std::wstring> const& dump_file_options::filter_values(std::wstring const& option) const
{
    auto const it = filter_values_.find(option);
    if (it != filter_values_.end())
    {
        return it->second;
    }

    return g_empty_values;
}
