// ValidateHeapEntries.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <array>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#define NOMINMAX 1
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable : 4100 4458)
#include <lyra/lyra.hpp>
#pragma warning(pop)

#include "ResultSet.h"
#include "symbol_engine_ui.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/get_last_address.h"
#include "DbgHelpUtils/heap_node_type.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heaps_options.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/process_heap_graph.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/statistic_view_options.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/system_module_list.h"

namespace ConsoleForeground
{
    constexpr WORD BLACK = 0;
    constexpr WORD DARKBLUE = FOREGROUND_BLUE;
    constexpr WORD DARKGREEN = FOREGROUND_GREEN;
    constexpr WORD DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD DARKRED = FOREGROUND_RED;
    constexpr WORD DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
    constexpr WORD DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
    constexpr WORD DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD GRAY = FOREGROUND_INTENSITY;
    constexpr WORD BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
    constexpr WORD GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
    constexpr WORD CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD RED = FOREGROUND_INTENSITY | FOREGROUND_RED;
    constexpr WORD MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
    constexpr WORD YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
    constexpr WORD WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
}

[[nodiscard]] std::wstring to_wstring(dlg_help_utils::heap::heap_node_type const value)
{
    using namespace std::string_literals;
    using dlg_help_utils::heap::heap_node_type;

    switch(value)
    {
        case heap_node_type::nt_heap_lfh_entry: return L"NT LFH Heap"s;
        case heap_node_type::nt_heap_segment_entry: return L"NT Heap Segment Entry"s;
        case heap_node_type::nt_heap_virtual_entry: return L"NT Heap Virtual Entry"s;
        case heap_node_type::segment_backend_entry: return L"Segment Heap Backend Entry"s;
        case heap_node_type::segment_entry: return L"Segment Heap Entry"s;
        case heap_node_type::segment_lfh_entry: return L"Segment Heap LFH Entry"s;
        case heap_node_type::segment_large_entry: return L"Segment Heap Large Entry"s;
        case heap_node_type::dph_entry: return L"DPH Entry"s;
        case heap_node_type::dph_virtual_entry: return L"DPH Virtual Entry"s;
        case heap_node_type::memory_range: return L"Memory Range"s;

    }

    return L"unknown"s;
}

bool on_process_heap_match_crt_entry(std::wostream& log
    , dlg_help_utils::heap::crt_entry const& crt_entry
    , uint64_t const user_address
    , dlg_help_utils::size_units::base_16::bytes const user_size
    , dlg_help_utils::heap::block_range_match_result const match
    , dlg_help_utils::heap::heap_node_type const node_type
    , std::streamsize const hex_length)
{
    using dlg_help_utils::stream_hex_dump::to_hex;
    auto successful = false;
    switch(match)
    {
    case dlg_help_utils::heap::block_range_match_result::block_match:
        log << std::format(L"INFO: block [{0}-{1}({2})] ({3}) matches CRT Entry [{4}:{5}-{6}({7})]\n"
            , to_hex(user_address, hex_length)
            , to_hex(user_address + user_size.count(), hex_length)
            , to_hex(user_size)
            , to_wstring(node_type)
            , to_hex(crt_entry.entry_address(), hex_length)
            , locale_formatting::to_wstring(crt_entry.request_number())
            , to_hex(crt_entry.end_entry_address(), hex_length)
            , to_hex(crt_entry.entry_size()));
        successful = true;
        break;

    case dlg_help_utils::heap::block_range_match_result::block_contains:
        log << std::format(L"WARNING: block [{0}-{1}({2})] ({3}) contains CRT Entry [{4}:{5}-{6}({7})]\n"
            , to_hex(user_address, hex_length)
            , to_hex(user_address + user_size.count(), hex_length)
            , to_hex(user_size)
            , to_wstring(node_type)
            , to_hex(crt_entry.entry_address(), hex_length)
            , locale_formatting::to_wstring(crt_entry.request_number())
            , to_hex(crt_entry.end_entry_address(), hex_length)
            , to_hex(crt_entry.entry_size()));
        successful = true;
        break;

    case dlg_help_utils::heap::block_range_match_result::user_contains_block:
        log << std::format(L"ERROR: CRT Entry [{4}:{5}-{6}({7})] contains block [{0}-{1}({2})] ({3})\n"
            , to_hex(user_address, hex_length)
            , to_hex(user_address + user_size.count(), hex_length)
            , to_hex(user_size)
            , to_wstring(node_type)
            , to_hex(crt_entry.entry_address(), hex_length)
            , locale_formatting::to_wstring(crt_entry.request_number())
            , to_hex(crt_entry.end_entry_address(), hex_length)
            , to_hex(crt_entry.entry_size()));
        break;

    case dlg_help_utils::heap::block_range_match_result::block_partially_contains:
        log << std::format(L"ERROR: block [{0}-{1}({2})] ({3}) partially contains CRT Entry [{4}:{5}-{6}({7})]\n"
            , to_hex(user_address, hex_length)
            , to_hex(user_address + user_size.count(), hex_length)
            , to_hex(user_size)
            , to_wstring(node_type)
            , to_hex(crt_entry.entry_address(), hex_length)
            , locale_formatting::to_wstring(crt_entry.request_number())
            , to_hex(crt_entry.end_entry_address(), hex_length)
            , to_hex(crt_entry.entry_size()));
        break;

    case dlg_help_utils::heap::block_range_match_result::block_no_match:
        successful = true;
        break;
    }

    return successful;
}

struct base_dump_file
{
    base_dump_file(std::wstring const& dump_filename, std::wostream* o_log, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, dlg_help_utils::heap::process_heaps_options const& options, dlg_help_utils::heap::system_module_list const& system_module_list, dlg_help_utils::heap::statistic_views::statistic_view_options const& statistic_view_options)
    : dump_file{open_mini_dump_file(dump_filename, o_log)}
    , heaps{dump_file, cache, symbol_engine, options, system_module_list, statistic_view_options}
    , crt_heap{cache, heaps.peb()}
    {
    }

    dlg_help_utils::cache_manager cache;
    dlg_help_utils::mini_dump dump_file;
    dlg_help_utils::heap::process_heaps heaps;
    dlg_help_utils::heap::crt_heap crt_heap;

private:
    static [[nodiscard]] dlg_help_utils::mini_dump open_mini_dump_file(std::wstring const& dump_filename, std::wostream* o_log)
    {
        dlg_help_utils::mini_dump dump_file{dump_filename};
        dump_file.open_mini_dump();
        if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
        {
            *o_log << std::format(L"ERROR: invalid base dmp file [{}]\n", dump_filename);
            throw std::runtime_error{"invalid base dmp file"};
        }

        return dump_file;
    }
};

[[nodiscard]] std::unique_ptr<base_dump_file> make_base_heap(std::wstring const& dump_filename, std::wostream* o_log, dlg_help_utils::heap::process_heaps& heaps, dlg_help_utils::heap::crt_heap& crt_heap, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, dlg_help_utils::heap::process_heaps_options const& options, dlg_help_utils::heap::system_module_list const& system_module_list, dlg_help_utils::heap::statistic_views::statistic_view_options const& statistic_view_options)
{
    if(dump_filename.empty())
    {
        return {};
    }

    try
    {
        auto rv = std::make_unique<base_dump_file>(dump_filename, o_log, symbol_engine, options, system_module_list, statistic_view_options);
        heaps.set_base_diff_filter(rv->heaps);
        crt_heap.set_base_diff_filter(rv->crt_heap);
        return rv;
    }
    catch(std::runtime_error const&)
    {
        return {};
    }
}

std::wstring get_crt_entry_description(dlg_help_utils::heap::process_heap_entry const& heap_entry)
{
    using namespace std::string_literals;
    return heap_entry.is_crt_heap_entry() ? L" mapped to CRT Entry"s : std::wstring{};
}

[[nodiscard]] bool check_allocation_range(Allocation const& allocation, dlg_help_utils::heap::process_heap_entry const& heap_entry, std::wostream* o_log, std::streamsize const hex_length)
{
    switch(heap_entry.match_range(allocation.pointer, dlg_help_utils::size_units::base_16::bytes{allocation.size}))
    {
    case dlg_help_utils::heap::block_range_match_result::block_match:
        *o_log << std::format(L"INFO: found json allocation [{0}] of [{1}/{2}] matches heap allocation block [{3}] of [{4}/{5}] - {6}{7}\n"
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
            , locale_formatting::to_wstring(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_address(), hex_length)
            , locale_formatting::to_wstring(heap_entry.user_requested_size().count())
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_requested_size().count())
            , to_wstring(heap_entry.get_heap_entry_type())
            , get_crt_entry_description(heap_entry));
        return true;

    case dlg_help_utils::heap::block_range_match_result::block_contains:
        *o_log << std::format(L"WARNING: found json allocation [{0}] of [{1}/{2}] but dmp heap allocation block is contained in [{3}] of [{4}/{5}] - {6}{7}\n"
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
            , locale_formatting::to_wstring(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_address(), hex_length)
            , locale_formatting::to_wstring(heap_entry.user_requested_size().count())
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_requested_size().count())
            , to_wstring(heap_entry.get_heap_entry_type())
            , get_crt_entry_description(heap_entry));
        return true;

    case dlg_help_utils::heap::block_range_match_result::user_contains_block:
        *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] but user allocation contains the dmp heap allocation block in [{3}] of [{4}/{5}] - {6}{7}\n"
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
            , locale_formatting::to_wstring(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_address(), hex_length)
            , locale_formatting::to_wstring(heap_entry.user_requested_size().count())
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_requested_size().count())
            , to_wstring(heap_entry.get_heap_entry_type())
            , get_crt_entry_description(heap_entry));
        return false;

    case dlg_help_utils::heap::block_range_match_result::block_partially_contains:
        *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] but dmp heap allocation block is partially contained in [{3}] of [{4}/{5}] - {6}{7}\n"
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
            , locale_formatting::to_wstring(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_address(), hex_length)
            , locale_formatting::to_wstring(heap_entry.user_requested_size().count())
            , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_requested_size().count())
            , to_wstring(heap_entry.get_heap_entry_type())
            , get_crt_entry_description(heap_entry));
        return false;

    case dlg_help_utils::heap::block_range_match_result::block_no_match:
        break;
    }

    *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] not matched but dmp heap allocation block closest match is [{3}] of [{4}/{5}] - {6}{7}\n"
        , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
        , locale_formatting::to_wstring(allocation.size)
        , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
        , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_address(), hex_length)
        , locale_formatting::to_wstring(heap_entry.user_requested_size().count())
        , dlg_help_utils::stream_hex_dump::to_hex(heap_entry.user_requested_size().count())
        , to_wstring(heap_entry.get_heap_entry_type())
        , get_crt_entry_description(heap_entry));
    return false;
}

bool is_allocation_matched(dlg_help_utils::heap::process_heap_entry const& heap_entry, dlg_help_utils::heap::crt_entry const& crt_entry)
{
    switch (heap_entry.match_range(crt_entry.user_address(), crt_entry.data_size()))
    {
    case dlg_help_utils::heap::block_range_match_result::block_match:
    case dlg_help_utils::heap::block_range_match_result::block_contains:
        return true;

    case dlg_help_utils::heap::block_range_match_result::user_contains_block:
    case dlg_help_utils::heap::block_range_match_result::block_partially_contains:
    case dlg_help_utils::heap::block_range_match_result::block_no_match:
        break;
    }

    return false;
}

[[nodiscard]] bool find_crt_allocation_in_heap_map(std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& heap_allocations, dlg_help_utils::heap::crt_entry const& crt_entry)
{
    if(const auto it = heap_allocations.lower_bound(crt_entry.user_address()); it != heap_allocations.end())
    {
        if(is_allocation_matched(it->second, crt_entry))
        {
            return true;
        }
    }

    return false;
}

std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> make_allocation_map(std::wostream* o_log, dlg_help_utils::heap::process_heaps const& heaps, bool& successful)
{
    std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> heap_allocations;
    for (auto const& entry : heaps.entries())
    {
        if (entry.user_address() == 0)
        {
            *o_log << L"ERROR: CRT allocation == nullptr\n";
            successful = false;
        }
        else
        {
            heap_allocations.insert(std::make_pair(dlg_help_utils::get_last_address(entry), entry));
        }
    }

    return heap_allocations;
}

std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> make_free_allocation_map(dlg_help_utils::heap::process_heaps const& heaps)
{
    std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> heap_free_entries;
    for(auto const& entry : heaps.free_entries())
    {
        heap_free_entries.insert(std::make_pair(dlg_help_utils::get_last_address(entry), entry));
    }
    return heap_free_entries;
}

bool validate_allocations(std::vector<Allocation> const& allocations
    , dlg_help_utils::heap::process_heaps const& heaps
    , dlg_help_utils::heap::crt_heap const& crt_heap
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& all_heap_allocations
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& heap_allocations
    , std::streamsize const hex_length
    , bool const stacktrace
    , bool const skip_fake_offset_check
    , std::wostream* o_log)
{
    bool successful = true;

    for(auto const& allocation : allocations)
    {
        if(heap_allocations.empty())
        {
            if(allocation.allocated)
            {
                *o_log << std::format(L"ERROR: can't find allocation [{}]\n", dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length));
                successful = false;
            }
            continue;
        }

        auto it = std::ranges::find_if(heap_allocations, [&allocation](auto const& entry) { return entry.second.contains_address_range(allocation.pointer, dlg_help_utils::size_units::base_16::bytes{allocation.size}); });
        auto found_it = it != heap_allocations.end();
        if (!found_it)
        {
            it = std::ranges::find_if(all_heap_allocations, [&allocation](auto const& entry) { return entry.second.contains_address_range(allocation.pointer, dlg_help_utils::size_units::base_16::bytes{allocation.size}); });
            found_it = it != all_heap_allocations.end();

            if (found_it)
            {
                *o_log << std::format(L"INFO: Allocation Entry [{0}] of [{1}/{2}] not found in diff allocation list but in all allocation list\n"
                                      , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                      , locale_formatting::to_wstring(allocation.size)
                                      , dlg_help_utils::stream_hex_dump::to_hex(allocation.size));
            }
        }

        if (!found_it)
        {
            if(allocation.allocated)
            {
                auto const it_closest = heap_allocations.lower_bound(allocation.pointer);

                // this happens when the base dmp has a allocation address/size that is reused between the first dmp and the second dump and
                // we don't have any record of it and it's not a crt heap (i.e. we can't tell they are different by the request number) then
                // we can't do anything about it as it looks the same... the only reason we can spot this unexpected situation is because
                // we have the json report of what you expected to have allocated.
                if(!crt_heap.is_using_crt_heap() && heaps.is_address_filtered(allocation.pointer, dlg_help_utils::size_units::base_16::bytes{allocation.size}))
                {
                    *o_log << std::format(L"WARNING: [{0}] of [{1}/{2}] is a filtered entry, the address/size is reused from the base dmp file\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                          , locale_formatting::to_wstring(allocation.size)
                                          , dlg_help_utils::stream_hex_dump::to_hex(allocation.size));
                }
                else
                {
                    *o_log << std::format(L"ERROR: can't find json allocation [{0}] of [{1}/{2}], closest dmp heap allocation is [{3}] of [{4}/{5}]\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                          , locale_formatting::to_wstring(allocation.size)
                                          , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
                                          , dlg_help_utils::stream_hex_dump::to_hex(it_closest->second.user_address(), hex_length)
                                          , locale_formatting::to_wstring(it_closest->second.user_requested_size().count())
                                          , dlg_help_utils::stream_hex_dump::to_hex(it_closest->second.user_requested_size().count()));
                    successful = false;
                }
            }
        }
        else if(!allocation.allocated)
        {
            *o_log << std::format(L"INFO: found json allocation [{0}] of [{1}/{2}] even though it's free(!), found dmp heap allocation is [{3}] of [{4}/{5}]\n"
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                  , locale_formatting::to_wstring(allocation.size)
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
                                  , dlg_help_utils::stream_hex_dump::to_hex(it->second.user_address(), hex_length)
                                  , locale_formatting::to_wstring(it->second.user_requested_size().count())
                                  , dlg_help_utils::stream_hex_dump::to_hex(it->second.user_requested_size().count()));
        }
        else if(!check_allocation_range(allocation, it->second, o_log, hex_length))
        {
            successful = false;
        }
        else if(stacktrace && it->second.allocation_stack_trace().empty())
        {
            *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] but dmp heap allocation has no expected stack trace\n"
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                  , locale_formatting::to_wstring(allocation.size)
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.size));
            successful = false;
        }
        else
        {
            size_t length{0};
            for(auto stream = it->second.user_data(allocation.pointer, dlg_help_utils::size_units::base_16::bytes{ allocation.size });
                auto [range_buffer, range_length] : stream.ranges())
            {
                auto* start_data = static_cast<char const*>(range_buffer);
                auto* data = start_data;
                auto* end_data = data + range_length;

                auto check_data = true;
                if(!skip_fake_offset_check && range_length > sizeof(uint32_t))
                {
                    uint32_t constexpr fake_offset{0x20};
                    if(memcmp(start_data, &fake_offset, sizeof fake_offset) != 0)
                    {
                        uint32_t offset{0};
                        memcpy(&offset, start_data, sizeof fake_offset);
                        *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] but failed fake offset of [{3} != {4}] compared to dmp allocation\n"
                                              , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                              , locale_formatting::to_wstring(allocation.size)
                                              , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
                                              , dlg_help_utils::stream_hex_dump::to_hex(offset)
                                              , dlg_help_utils::stream_hex_dump::to_hex(fake_offset));
                        successful = false;
                        check_data = false;
                    }

                    data += sizeof fake_offset;
                }

                if(check_data)
                {
                    while(data < end_data)
                    {
                        if(*data != allocation.fill_char)
                        {
                            *o_log << std::format(L"ERROR: found json allocation [{0}] of [{1}/{2}] but failed fill compare of [{3}] compared to dmp allocation of [{4}] @ [{5}]\n"
                                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                                  , locale_formatting::to_wstring(allocation.size)
                                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.size)
                                                  , allocation.fill_char
                                                  , *data
                                                  , dlg_help_utils::stream_hex_dump::to_hex(length + (data - start_data)));
                            successful = false;
                            break;
                        }
                        ++data;
                    }
                }
                length += range_length;
            }
        }
    }

    return successful;
}

bool validate_crt_entries(dlg_help_utils::heap::crt_heap const& crt_heap
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& all_heap_allocations
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& all_heap_free_allocations
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& heap_allocations
    , std::map<uint64_t, dlg_help_utils::heap::process_heap_entry> const& heap_free_allocations
    , std::wostream* o_log
    , std::streamsize const hex_length)
{
    auto successful = true;

    // valid CRT Entries are all found
    for (auto const& entry : crt_heap.entries())
    {
        if(entry.block_use())
        {
            if(!find_crt_allocation_in_heap_map(heap_allocations, entry))
            {
                if(find_crt_allocation_in_heap_map(all_heap_allocations, entry))
                {
                    *o_log << std::format(L"INFO: CRT Allocation Entry [{0}] of [{1}/{2}] not found in diff allocation list but in all allocation list\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.user_address(), hex_length)
                                          , locale_formatting::to_wstring(entry.data_size().count())
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.data_size().count()));
                }
                else
                {
                    *o_log << std::format(L"ERROR: CRT Allocation Entry [{0}] of [{1}/{2}] not found\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.user_address(), hex_length)
                                          , locale_formatting::to_wstring(entry.data_size().count())
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.data_size().count()));
                    successful = false;
                }
            }
        }
        else
        {
            if (!find_crt_allocation_in_heap_map(heap_free_allocations, entry))
            {
                if (find_crt_allocation_in_heap_map(all_heap_free_allocations, entry))
                {
                    *o_log << std::format(L"INFO: CRT Free Entry [{0}] of [{1}/{2}] not found in diff free allocation list but in all free allocation list\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.user_address(), hex_length)
                                          , locale_formatting::to_wstring(entry.data_size().count())
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.data_size().count()));
                }
                else
                {
                    *o_log << std::format(L"ERROR: CRT Free Entry [{0}] of [{1}/{2}] not found\n"
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.user_address(), hex_length)
                                          , locale_formatting::to_wstring(entry.data_size().count())
                                          , dlg_help_utils::stream_hex_dump::to_hex(entry.data_size().count()));
                    successful = false;
                }
            }
        }
    }

    return successful;
}

bool validate_allocation_graph_entries_are_not_marked_system(std::vector<Allocation> const& allocations
    , dlg_help_utils::mini_dump const& dump_file
    , dlg_help_utils::heap::process_heaps const& heaps
    , dlg_help_utils::heap::system_module_list const& system_module_list
    , std::streamsize const hex_length
    , std::wostream* o_log)
{
    auto successful = true;

    // validate that all allocations are not marked as system
    dlg_help_utils::heap::process_heap_graph graph{dump_file, heaps, system_module_list};

    graph.generate_graph();

    std::map<uint64_t, dlg_help_utils::heap::allocation_graph::process_heap_graph_heap_entry> graph_allocation_nodes;
    for (auto const& node : graph.nodes())
    {
        if(std::holds_alternative<dlg_help_utils::heap::allocation_graph::process_heap_graph_heap_entry>(node))
        {
            auto const& heap_node = std::get<dlg_help_utils::heap::allocation_graph::process_heap_graph_heap_entry>(node);
            graph_allocation_nodes.insert(std::make_pair(heap_node.heap_entry().user_address(), heap_node));
        }
    }

    for (auto const& allocation : allocations)
    {
        if(!allocation.allocated)
        {
            continue;
        }

        if(auto const it = graph_allocation_nodes.find(allocation.pointer);
            it == graph_allocation_nodes.end())
        {
            *o_log << std::format(L"ERROR: Allocation Entry [{0}] of [{1}/{2}] size not found in graph allocations list\n"
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                  , locale_formatting::to_wstring(allocation.size)
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.size));
            successful = false;
        }
        else if(it->second.is_system_allocation())
        {
            *o_log << std::format(L"ERROR: Allocation Entry [{0}] of [{1}/{2}] size marked as system allocation\n"
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.pointer, hex_length)
                                  , locale_formatting::to_wstring(allocation.size)
                                  , dlg_help_utils::stream_hex_dump::to_hex(allocation.size));
            successful = false;
        }
    }

    return successful;
}

[[nodiscard]] bool validate_dump_file(bool stacktrace, bool skip_fake_offset_check, bool no_output, std::wstring const& dump_filename, std::wstring const& base_dump_filename, std::wostream* o_log, std::vector<Allocation> const& allocations)
{
    *o_log << std::format(L"Processing dmp [{}]\n", dump_filename);

    dlg_help_utils::mini_dump dump_file{dump_filename};
    dump_file.open_mini_dump();
    if(dump_file.type() != dlg_help_utils::dump_file_type::user_mode_dump)
    {
        *o_log << std::format(L"ERROR: invalid dmp file [{}]\n", dump_filename);
        return false;
    }


    using namespace dlg_help_utils::size_units::base_16;
    symbol_engine_ui ui{no_output};
    dlg_help_utils::dbg_help::symbol_engine symbol_engine{ui};
    dlg_help_utils::heap::system_module_list system_module_list;
    dlg_help_utils::heap::statistic_views::statistic_view_options statistic_view_options;
    dlg_help_utils::heap::process_heaps_options process_options;

    dlg_help_utils::cache_manager cache;
    dlg_help_utils::process::process_environment_block const peb{dump_file, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    bool successful = true;

    process_options.no_filter_heap_entries() = true;
    process_options.no_mark_all_crt_entries_as_system() = true;
    process_options.set_match_result_callback([o_log, &successful, hex_length](dlg_help_utils::heap::crt_entry const& crt_entry
        , uint64_t const user_address
        , bytes const user_size
        , dlg_help_utils::heap::block_range_match_result const match
        , dlg_help_utils::heap::heap_node_type const node_type) mutable
    {
        if(!on_process_heap_match_crt_entry(*o_log, crt_entry, user_address, user_size, match, node_type, hex_length))
        {
            successful = false;
        }
    });

    dlg_help_utils::heap::process_heaps heaps{dump_file, cache, symbol_engine, process_options, system_module_list, statistic_view_options};
    dlg_help_utils::heap::crt_heap crt_heap{ cache, heaps.peb() };

    auto const all_heap_allocations = make_allocation_map(o_log, heaps, successful);
    auto const all_heap_free_allocations = make_free_allocation_map(heaps);

    // ReSharper disable once CppTooWideScopeInitStatement
    auto base_heap = make_base_heap(base_dump_filename, o_log, heaps, crt_heap, symbol_engine, process_options, system_module_list, statistic_view_options);

    if(!base_dump_filename.empty() && !base_heap)
    {
        return true;
    }

    auto const heap_allocations = make_allocation_map(o_log, heaps, successful);
    auto const heap_free_allocations = make_free_allocation_map(heaps);

    if(!validate_allocations(allocations, heaps, crt_heap, all_heap_allocations, heap_allocations, hex_length, stacktrace, skip_fake_offset_check, o_log))
    {
        successful = false;
    }

    if(!validate_crt_entries(crt_heap, all_heap_allocations, all_heap_free_allocations, heap_allocations, heap_free_allocations, o_log, hex_length))
    {
        successful = false;
    }

    if(!validate_allocation_graph_entries_are_not_marked_system(allocations, dump_file, heaps, system_module_list, hex_length, o_log))
    {
        successful = false;
    }

    if(!no_output)
    {
        std::wcout << dump_filename << " : ";
        auto console = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(console, successful ? ConsoleForeground::GREEN : ConsoleForeground::RED);
        std::wcout << (successful ? L"Passed" : L"Failed");
        SetConsoleTextAttribute(console, ConsoleForeground::WHITE);
        std::wcout << L'\n';
    }

    return successful;
}

int main(int const argc, char* argv[])
{
    try
    {
        try
        {
            std::string dump_filename_1_l;
            std::string dump_filename_2_l;
            std::string log_filename_l;
            std::string json_filename_l;
            auto stacktrace{false};
            auto show_help{false};
            auto skip_fake_offset_check{false};
            auto no_output{false};
            auto cli = lyra::help(show_help)
                | lyra::opt( dump_filename_1_l, "filename" )["-1"]["--dmp1"]("first dump filename")
                | lyra::opt( dump_filename_2_l, "filename" )["-2"]["--dmp2"]("second dump filename")
                | lyra::opt( log_filename_l, "filename" )["-l"]["--log"]("log filename")
                | lyra::opt( json_filename_l, "filename" )["-j"]["--json"]("json filename")
                | lyra::opt( stacktrace)["-s"]["--stacktrace"]("expects allocation stack trace")
                | lyra::opt( skip_fake_offset_check)["--skip-fake-offset"]("skip fake offset check (assume older generated dmp test file)")
                | lyra::opt( no_output)["--no-output"]("don't display output to the console")
                ;

            if (auto const result = cli.parse({ argc, argv });
                !result)
            {
                std::cerr << std::format("Error in command line: {}\n", result.errorMessage());
                std::cerr << cli << '\n';
                return EXIT_FAILURE;
            }

            // Show the help when asked for.
            if (show_help)
            {
                std::cout << cli << '\n';
                return EXIT_SUCCESS;
            }

            auto const dump_filename_1 = dlg_help_utils::string_conversation::acp_to_wstring(dump_filename_1_l);
            auto const dump_filename_2 = dlg_help_utils::string_conversation::acp_to_wstring(dump_filename_2_l);
            auto const log_filename = dlg_help_utils::string_conversation::acp_to_wstring(log_filename_l);
            auto const json_filename = dlg_help_utils::string_conversation::acp_to_wstring(json_filename_l);

            if(dump_filename_1.empty())
            {
                std::cerr << "No dmp file specified to process\n";
                std::cout << cli << '\n';
                return EXIT_SUCCESS;
            }

            std::unique_ptr<std::wfstream> log;
            null_stream null_stream;
            std::wostream* o_log{no_output ? &null_stream : &std::wcout};
            if(!log_filename.empty())
            {
                log = std::make_unique<std::wfstream>(log_filename, std::ios_base::out | std::ios_base::app);
                if(log->bad())
                {
                    std::wcerr << std::format(L"failed to open log file: {}\n", log_filename);
                    return EXIT_FAILURE;
                }
                o_log = log.get();
            }

            std::fstream json_file{json_filename, std::ios_base::in};
            if(json_file.bad())
            {
                *o_log << std::format(L"failed to open json result set file: {}\n", json_filename);
                return EXIT_FAILURE;
            }

            std::stringstream buffer;
            buffer << json_file.rdbuf();
            json_file.close();
            auto json = std::move(buffer).str();

            JS::ParseContext context(json);
            ResultSet set;
            if (context.parseTo(set) != JS::Error::NoError)
            {
                *o_log << std::format(L"failed to parse json result set file: {0} with {1}\n", json_filename, dlg_help_utils::string_conversation::acp_to_wstring(context.makeErrorString()));
                return EXIT_FAILURE;
            }

            auto successful = validate_dump_file(stacktrace, skip_fake_offset_check, no_output, dump_filename_1, {}, o_log, set.first_allocations);
            if(successful && !dump_filename_2.empty())
            {
                successful = validate_dump_file(stacktrace, skip_fake_offset_check, no_output, dump_filename_2, dump_filename_1, o_log, set.second_allocations);
            }

            if(log)
            {
                log->close();
            }

            return successful ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        catch (std::exception const& e)
        {
            std::cerr << std::format("FATAL ERROR: {}\n", e.what());
        }
        catch (...)
        {
            std::cerr << "FATAL ERROR: Unknown exception\n";
        }
    }
    catch(...)
    {
    }
    return EXIT_FAILURE;
}
