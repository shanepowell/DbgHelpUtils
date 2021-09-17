#include "dump_file_options.h"

#include "DbgHelpUtils/mini_dump_stream_type.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#include <ranges>

using namespace std;

namespace
{
    std::vector<std::wstring> const g_empty_values;
}

lyra::cli dump_file_options::generate_options()
{
    return 
        lyra::opt( display_version_)["-v"]["--version"]("display version information")
        | lyra::opt( dump_header_)["-h"]["--header"]("dump file header")
        | lyra::opt( dump_files_raw_, "filename" )["-d"]["--dumpfile"]("dump files to open")
        | lyra::opt( continue_on_errors_)["-c"]["--continue"]("continue on errors")
        | lyra::opt( dump_streams_)["-s"]["--streams"]("dump streams")
        | lyra::opt( hex_dump_stream_data_)["-x"]["--hexdump"]("hex dump stream data")
        | lyra::opt( hex_dump_memory_data_)["-m"]["--memoryhexdump"]("limit hex dump memory data to size")
        | lyra::opt( limit_hex_dump_memory_size_raw_, "limitmemoryhexdump" )["--limitmemoryhexdump"]("limit hex dump memory data to size")
        | lyra::opt( dump_stream_indexes_, "streamindex" )["-i"]["--streamindex"]("dump stream indexes")
        | lyra::opt( dump_stream_types_raw_, "streamtype" )["-t"]["--streamtype"]("dump stream indexes")
        | lyra::opt( display_symbols_)["-y"]["--symbols"]("display stack trace symbols")
        | lyra::opt( filter_values_raw_, "filter" )["--filter"]("filter by supported values")
        | lyra::opt( debug_symbols_)["--symboldebug"]("debug load symbols")
        | lyra::opt( debug_load_symbols_memory_)["--symboldebugmemory"]("debug load symbols memory loading")
        | lyra::opt( generate_crc32_)["--crc32"]("generate crc32")
        | lyra::opt( symbol_types_raw_, "type" )["-p"]["--type"]("dump symbol type information")
        | lyra::opt( symbol_names_raw_, "symbol" )["--symbol"]("dump symbol information")
        | lyra::opt( dump_types_modules_raw_, "moduletype" )["--moduletypes"]("dump module symbol types")
        | lyra::opt( dump_address_types_raw_, "address" )["--address"]("dump address with type")
        | lyra::opt( debug_type_data_)["--typedebug"]("debug type data")
        | lyra::opt( debug_heap_data_)["--heapdebug"]("debug heap data")
        | lyra::opt( display_peb_)["--peb"]("process environment block")
        | lyra::opt( display_heap_)["--heap"]("heap data information")
        | lyra::opt( display_heap_entries_)["--heapentries"]("heap entries only")
        | lyra::opt( display_heap_statistics_)["--heapstats"]("heap statistics")
        | lyra::opt( display_crtheap_)["--crtheap"]("crtheap data information")
        | lyra::opt( display_stack_trace_database_)["--std"]("stack trace database")
    ;
}

void dump_file_options::process_raw_options()
{
    dump_files_ = convert_to_wstring(dump_files_raw_);

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
