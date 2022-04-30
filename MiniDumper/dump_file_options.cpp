#include "dump_file_options.h"

#include <map>
#include <numeric>
#include <ranges>

#pragma warning(push)
#pragma warning(disable : 4267 4457 4100)
#include <json_struct/include/json_struct.h>
#pragma warning(pop)

#include <fstream>

#include "DbgHelpUtils/join.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/process_heaps_statistic_view.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std;
using namespace std::string_literals;

namespace
{
    // ReSharper disable once CppVariableCanBeMadeConstexpr
    std::vector<std::wstring> const g_empty_values{};

    map<std::string, uint16_t> const g_heap_statistics_view_options
    {
        {"size"s, heap_statistics_view::by_size_frequency_view},
        {"range"s, heap_statistics_view::by_size_ranges_frequency_view},
        {"stack"s, heap_statistics_view::by_stacktrace_frequency_view},
        {"callsite"s, heap_statistics_view::by_application_callsite_frequency_view},
        {"all"s, heap_statistics_view::all_views}
    };

    map<std::string, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type> const g_view_sort_column
    {
        {"size"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::size},
        {"allocated_total"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::allocated_total},
        {"allocated_count"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::allocated_count},
        {"allocated_average"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::allocated_average},
        {"free_total"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::free_total},
        {"free_count"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::free_count},
        {"overhead_total"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::overhead_total},
        {"range_size_percent"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::range_size_percent},
        {"range_count_percent"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_column_type::range_count_percent}
    };

    map<std::string, dlg_help_utils::heap::process_heaps_statistic_view::sort_order_type> const g_view_sort_order
    {
        {"ascending"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_order_type::ascending},
        {"descending"s, dlg_help_utils::heap::process_heaps_statistic_view::sort_order_type::descending}
    };

    struct system_modules_json
    {
        std::vector<std::string> systemmodules;

        JS_OBJ(systemmodules);
    };
}

lyra::cli dump_file_options::generate_options()
{
    return 
        lyra::opt(display_version_)["-v"]["--version"]("display version information")
        | lyra::opt(dump_header_)["-r"]["--header"]("dump file header")
        | lyra::opt(dump_files_raw_, "filename" )["-d"]["--dumpfile"]("dump files to open")
        // ReSharper disable once StringLiteralTypo
        | lyra::opt(base_diff_dump_files_raw_, "filename" )["--basediffdumpfile"]("base diff dump files to open")
        | lyra::opt(continue_on_errors_)["-c"]["--continue"]("continue on errors")
        | lyra::opt(dump_streams_)["-s"]["--streams"]("dump streams")
        | lyra::opt(hex_dump_stream_data_)["-x"]["--hexdump"]("hex dump stream data")
        | lyra::opt(hex_dump_memory_data_)["-m"]["--memoryhexdump"]("limit hex dump memory data to size")
        | lyra::opt(limit_hex_dump_memory_size_raw_, "limitmemoryhexdump" )["--limitmemoryhexdump"]("limit hex dump memory data to size")
        | lyra::opt(dump_stream_indexes_, "streamindex" )["-i"]["--streamindex"]("dump stream indexes")
        | lyra::opt(dump_stream_types_raw_, "streamtype" )["-t"]["--streamtype"]("dump stream indexes")
        // ReSharper disable once StringLiteralTypo
        | lyra::opt(dump_all_stream_indexes_)["--dumpallstreams"]("dump all stream indexes")
        | lyra::opt(display_symbols_)["-y"]["--symbols"]("display stack trace symbols")
        | lyra::opt(display_stack_parameters_)["--parameters"]("display stack trace parameter symbols")
        | lyra::opt(display_stack_variables_)["--variables"]("display stack trace variables symbols")
        | lyra::opt(filter_values_raw_, "filter" )["--filter"]("filter by supported values")
        | lyra::opt(debug_symbols_)["--symboldebug"]("debug load symbols")
        | lyra::opt(debug_load_symbols_memory_)["--symboldebugmemory"]("debug load symbols memory loading")
        | lyra::opt(generate_crc32_)["--crc32"]("generate crc32")
        | lyra::opt(symbol_types_raw_, "type" )["-p"]["--type"]("dump symbol type information")
        | lyra::opt(symbol_names_raw_, "symbol" )["--symbol"]("dump symbol information")
        | lyra::opt(dump_types_modules_raw_, "moduletype" )["--moduletypes"]("dump module symbol types")
        | lyra::opt(dump_address_types_raw_, "address" )["--address"]("dump address with type")
        | lyra::opt(debug_type_data_)["--typedebug"]("debug type data")
        // ReSharper disable once StringLiteralTypo
        | lyra::opt(debug_type_parent_data_)["--typeparentdebug"]("debug type parent data")
        | lyra::opt(debug_heap_data_)["--heapdebug"]("debug heap data")
        | lyra::opt(display_peb_)["--peb"]("process environment block")
        | lyra::opt(display_heap_)["--heap"]("heap data information")
        | lyra::opt(display_heap_entries_)["--heapentries"]("heap entries only")
        | lyra::opt(display_crtheap_)["--crtheap"]("crtheap data information")
        | lyra::opt(display_stack_trace_database_)["--std"]("stack trace database")
        | lyra::opt(disable_symbol_load_cancel_keyboard_check_)["--disable-symbol-load-cancel-keyboard-check"]("disable the keyboard check when loading symbols")
        | lyra::opt(heap_statistics_raw_, dlg_help_utils::join(g_heap_statistics_view_options | std::views::keys, "|"sv))["--heapstat"]("display heap statistic").choices([](std::string const& value) { return g_heap_statistics_view_options.contains(value); })
        | lyra::opt(by_range_view_range_raw_, "range")["--viewrange"]("heap size statistic view bucket size")
        | lyra::opt(system_module_list_file_, "filename")["--systemmodules"]("json file holding the list of system modules")
        // ReSharper disable once StringLiteralTypo
        | lyra::opt(view_sort_column_raw_, dlg_help_utils::join(g_view_sort_column | std::views::keys, "|"sv))["--statsortcolumn"]("display heap statistic").choices([](std::string const& value) { return g_view_sort_column.contains(value); })
        // ReSharper disable once StringLiteralTypo
        | lyra::opt(view_sort_order_raw_, dlg_help_utils::join(g_view_sort_order | std::views::keys, "|"sv))["--statsortorder"]("display heap statistic").choices([](std::string const& value) { return g_view_sort_order.contains(value); })
        | lyra::opt(output_filename_raw_, "filename")["--out"]("output filename")
    ;
}

void dump_file_options::process_raw_options()
{
    dump_files_ = convert_to_wstring(dump_files_raw_);
    base_diff_dump_files_ = convert_to_wstring(base_diff_dump_files_raw_);
    output_filename_ = dlg_help_utils::string_conversation::acp_to_wstring(output_filename_raw_);
    output_filename_raw_.clear();

    if (!dump_stream_types_raw_.empty())
    {
        dump_stream_types_.reserve(dump_stream_types_raw_.size());
        for (auto const& stream_type : dump_stream_types_raw_)
        {
            dump_stream_types_.emplace_back(dlg_help_utils::mini_dump_stream_type::from_string(dlg_help_utils::string_conversation::acp_to_wstring(stream_type)));
        }
        dump_stream_types_raw_.clear();
    }

    if (!filter_values_raw_.empty())
    {
        for (auto const& filter_value : filter_values_raw_ | std::views::transform([](std::string const& value) { return dlg_help_utils::string_conversation::acp_to_wstring(value); }))
        {
            auto const pos = filter_value.find_first_of('=');
            if (pos == std::wstring::npos)
            {
                throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Filter value: [{}] format incorrect. Expecting <name>=<value> format", filter_value)};
            }

            auto name = filter_value.substr(0, pos);
            auto value = filter_value.substr(pos + 1);

            if (name.empty() || value.empty())
            {
                throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Filter value: [{}] format incorrect. Expecting <name>=<value> format", filter_value)};
            }

            filter_values_[name].emplace_back(std::move(value));
        }
        filter_values_raw_.clear();
    }

    if(!limit_hex_dump_memory_size_raw_.empty())
    {
        limit_hex_dump_memory_size_ = dlg_help_utils::size_units::base_16::from_wstring(dlg_help_utils::string_conversation::acp_to_wstring(limit_hex_dump_memory_size_raw_)).count();
        limit_hex_dump_memory_size_raw_.clear();
    }

    symbol_types_ = convert_to_wstring(symbol_types_raw_);
    symbol_names_ = convert_to_wstring(symbol_names_raw_);
    dump_types_modules_ = convert_to_wstring(dump_types_modules_raw_);
    dump_address_types_ = convert_to_wstring(dump_address_types_raw_);

    auto view_options = heap_statistics_raw_ | std::views::transform([](std::string const& value) { return g_heap_statistics_view_options.at(value); });
    heap_statistics_views_ = std::accumulate(std::begin(view_options), std::end(view_options), static_cast<uint16_t>(0), [](uint16_t const options, uint16_t const value) -> uint16_t { return options | value; });
    heap_statistics_raw_.clear();

    if(!by_range_view_range_raw_.empty())
    {
        statistic_view_options_.range_size() = dlg_help_utils::size_units::base_16::from_wstring(dlg_help_utils::string_conversation::acp_to_wstring(by_range_view_range_raw_));
        by_range_view_range_raw_.clear();
    }

    if(!system_module_list_file_.empty())
    {
        std::fstream json_file{system_module_list_file_, std::ios_base::in};
        if(json_file.bad())
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"failed to open json result set file: {}", dlg_help_utils::string_conversation::acp_to_wstring(system_module_list_file_))};
        }

        std::stringstream buffer;
        buffer << json_file.rdbuf();
        json_file.close();
        const auto json = std::move(buffer).str();

        JS::ParseContext context(json);
        system_modules_json values;
        if (context.parseTo(values) != JS::Error::NoError)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"failed to parse json system modules file: {0} with {1}", dlg_help_utils::string_conversation::acp_to_wstring(system_module_list_file_), dlg_help_utils::string_conversation::acp_to_wstring(context.makeErrorString()))};
        }

        system_module_list_ = dlg_help_utils::heap::statistic_views::system_module_list{convert_to_wstring(values.systemmodules)};
        system_module_list_file_.clear();
    }

    if(!view_sort_column_raw_.empty())
    {
        statistic_view_options_.view_sort_column() = g_view_sort_column.at(view_sort_column_raw_);
        view_sort_column_raw_.clear();
    }

    if(!view_sort_order_raw_.empty())
    {
        statistic_view_options_.view_sort_order() = g_view_sort_order.at(view_sort_order_raw_);
        view_sort_order_raw_.clear();
    }
}

std::wostream& dump_file_options::get_log_stream()
{
    if(output_filename_.empty())
    {
        return wcout;
    }

    if(!log_file_.is_open())
    {
        log_file_.open(output_filename_, ios_base::app);
    }

    return log_file_;
}

std::vector<std::wstring> dump_file_options::convert_to_wstring(std::vector<std::string>& raw)
{
    std::vector<std::wstring> rv;
    if(!raw.empty())
    {
        rv.reserve(raw.size());
        std::ranges::copy(raw | std::views::transform([](std::string const& value) { return dlg_help_utils::string_conversation::acp_to_wstring(value); }), std::back_inserter(rv));
        raw.clear();
    }

    return rv;
}

uint64_t dump_file_options::hex_dump_memory_size(uint64_t const size) const
{
    if(limit_hex_dump_memory_size() > 0)
    {
        return std::min(limit_hex_dump_memory_size(), size);
    }
    return size;
}

dlg_help_utils::stream_stack_dump::dump_stack_options::options dump_file_options::display_stack_options() const
{
    using namespace dlg_help_utils::stream_stack_dump;
    uint32_t options{0};

    if(display_stack_parameters_)
    {
        options |= dump_stack_options::DisplayStackParameters;
    }

    if(display_stack_variables_)
    {
        options |= dump_stack_options::DisplayStackVariables;
    }

    return static_cast<dump_stack_options::options>(options);
}

std::vector<std::wstring> const& dump_file_options::filter_values(std::wstring const& option) const
{
    if (auto const it = filter_values_.find(option); it != filter_values_.end())
    {
        return it->second;
    }

    return g_empty_values;
}
