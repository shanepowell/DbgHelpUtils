#include "allocation_stack_trace_helper.h"

#include <numeric>
// ReSharper disable once CppUnusedIncludeDirective
#include <ranges>

#include "hash_combine.h"
#include "mini_dump_stack_walk.h"
#include "process_heap_entry.h"
#include "system_module_list.h"

using namespace std;

namespace dlg_help_utils::heap::statistic_views
{
    namespace
    {
        bool is_application_module(stream_stack_dump::mini_dump_stack_walk const& walker, system_module_list const& module_list, uint64_t const address)
        {
            if(auto const symbol_info = stream_stack_dump::mini_dump_stack_walk::find_symbol_info(address, walker.module_list(), walker.unloaded_module_list(), walker.symbol_engine());
                symbol_info.has_value())
            {
                return !module_list.is_system_module(symbol_info.value().module_name);
            }

            return false;
        }

        template<typename It>
        uint64_t create_stack_trace_key(It first, It last)
        {
            return std::accumulate(first, last, static_cast<uint64_t>(0), [](uint64_t seed, uint64_t const value) { hash_combine(seed, value); return seed; });
        }

    }

    allocation_stack_trace_helper::allocation_stack_trace_helper(stream_stack_dump::mini_dump_stack_walk const& walker, system_module_list const& system_module_list)
    : walker_{walker}
    , system_module_list_{system_module_list}
    {
    }

    std::optional<dbg_help::symbol_address_info> allocation_stack_trace_helper::find_common_allocation_callsite(process_heap_entry const& entry) const
    {
        if(entry.allocation_stack_trace().empty())
        {
            return std::nullopt;
        }

        auto is_application_module_filter = [this](uint64_t const& address) { return is_application_module(walker_, system_module_list_, address); };

        //
        // find the first return stack function return pointer that doesn't exist in the "system" modules as listed in system_module_list_
        //
        if(auto application_call_sites = entry.allocation_stack_trace() | std::views::filter(is_application_module_filter);
            std::ranges::begin(application_call_sites) != std::ranges::end(application_call_sites))
        {
            return stream_stack_dump::mini_dump_stack_walk::find_symbol_info(*std::ranges::begin(application_call_sites), walker_.module_list(), walker_.unloaded_module_list(), walker_.symbol_engine()).value();
        }

        return std::nullopt;
    }

    std::optional<dbg_help::symbol_address_info> allocation_stack_trace_helper::find_common_allocation_callsite(std::vector<process_heap_entry> const& entries) const
    {
        if(entries.empty())
        {
            return std::nullopt;
        }

        //
        // find the first "common" allocation callsite that exists in "ALL" entries, it goes down the first entry call stack
        //

        if(entries.size() == 1)
        {
            return find_common_allocation_callsite(entries.front());
        }

        auto is_application_module_filter = [this](uint64_t const& address) { return is_application_module(walker_, system_module_list_, address); };

        // ReSharper disable once CppTooWideScopeInitStatement
        auto find_application_module_in_all_other_stacks_filter = [&entries](uint64_t const address)
        {
            auto find_application_module_in_other_stacks_filter = [address](process_heap_entry const& entry) { return ranges::any_of(entry.allocation_stack_trace(), [address](uint64_t const a) { return a == address; }); };
            return std::ranges::all_of(entries | std::views::drop(1), find_application_module_in_other_stacks_filter);
        };

        if(auto application_call_sites = entries.front().allocation_stack_trace() | std::views::filter(is_application_module_filter) | std::views::filter(find_application_module_in_all_other_stacks_filter);
            std::ranges::begin(application_call_sites) != std::ranges::end(application_call_sites))
        {
            return stream_stack_dump::mini_dump_stack_walk::find_symbol_info(*std::ranges::begin(application_call_sites), walker_.module_list(), walker_.unloaded_module_list(), walker_.symbol_engine()).value();
        }

        return std::nullopt;
    }

    std::vector<uint64_t> allocation_stack_trace_helper::find_common_allocation_stack_trace(std::optional<dbg_help::symbol_address_info> const& common_allocation_callsite, std::vector<process_heap_entry> const& entries) const
    {
        if(!common_allocation_callsite.has_value())
        {
            return {};
        }

        // for the "common" call site stack function return pointer, find the most common call stack
        struct common_stack_data
        {
            std::vector<uint64_t> stack_trace;
            size_t count{1};
        };
        std::map<uint64_t, common_stack_data> common_stacks;

        for(auto const& entry : entries)
        {
            if(auto it = std::ranges::find(entry.allocation_stack_trace(), common_allocation_callsite.value().address);
                it != entry.allocation_stack_trace().end())
            {
                auto key = create_stack_trace_key(it, entry.allocation_stack_trace().end());

                if(auto const find_it = common_stacks.find(key);
                    find_it != common_stacks.end())
                {
                    ++find_it->second.count;
                }
                else
                {
                    common_stack_data data;
                    std::copy(it, entry.allocation_stack_trace().end(), std::back_inserter(data.stack_trace));
                    common_stacks.insert(std::make_pair(key, std::move(data)));
                }
            }
        }

        if(common_stacks.empty())
        {
            return {};
        }

#if _MSC_VER > 1929
        // latest vs2019 fails to compile this line, the result of std::ranges::max_element is std::ranges::dangling
        return (*std::ranges::max_element(common_stacks | std::views::values, [](common_stack_data const& a, common_stack_data const& b) { return a.count < b.count; })).stack_trace;

#else
        {
            auto r = common_stacks | std::views::values;
            return (*std::max_element(std::begin(r), std::end(r), [](common_stack_data const& a, common_stack_data const& b) { return a.count < b.count; })).stack_trace;
        }
#endif
    }
}
