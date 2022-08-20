#include "process_heaps.h"

#include <algorithm>

#include "common_symbol_utils.h"
#include "crt_entry.h"
#include "crt_heap.h"
#include "dph_entry.h"
#include "dph_heap.h"
#include "get_last_address.h"
#include "heap_entry.h"
#include "heap_node_type.h"
#include "heap_lfh_affinity_slot.h"
#include "heap_lfh_bucket.h"
#include "heap_lfh_context.h"
#include "heap_lfh_entry.h"
#include "heap_lfh_subsegment.h"
#include "heap_page_segment.h"
#include "heap_segment.h"
#include "heap_segment_context.h"
#include "heap_subsegment.h"
#include "heap_virtual_block.h"
#include "heap_vs_context.h"
#include "heap_vs_entry.h"
#include "heap_vs_subsegment.h"
#include "large_alloc_entry.h"
#include "lfh_heap.h"
#include "lfh_segment.h"
#include "memory_range.h"
#include "nt_heap.h"
#include "page_range_descriptor.h"
#include "process_heaps_options.h"
#include "process_heaps_statistics.h"
#include "process_heap_entry.h"
#include "segment_heap.h"

namespace dlg_help_utils::heap
{
    process_heaps::process_heaps(mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine, process_heaps_options const& options, system_module_list const& system_module_list, statistic_views::statistic_view_options const& statistic_view_options)
    : options_{&options}
    , cache_manager_{&cache}
    , peb_{ mini_dump, cache, symbol_engine }
    , system_module_list_{&system_module_list}
    , statistic_view_options_{&statistic_view_options}
    {
        peb_.gather_system_area_addresses(system_area_addresses_);

        for (uint32_t heap_index = 0; heap_index < peb().number_of_heaps(); ++heap_index)
        {
            if (auto const nt_heap = peb().nt_heap(heap_index); nt_heap.has_value())
            {
                if(!system_area_addresses_.contains(nt_heap.value().nt_heap_address()))
                {
                    system_area_addresses_.insert(nt_heap.value().nt_heap_address());
                }
            }
            else if (auto const segment_heap = peb().segment_heap(heap_index); segment_heap.has_value())
            {
                if(!system_area_addresses_.contains(segment_heap.value().segment_heap_address()))
                {
                    system_area_addresses_.insert(segment_heap.value().segment_heap_address());
                }
            }
        }

        for (auto const& heap : dph_heap::dph_heaps(cache, peb()))
        {
            if(!system_area_addresses_.contains(heap.address()))
            {
                system_area_addresses_.insert(heap.address());
            }
        }
    }

    void process_heaps::set_base_diff_filter(process_heaps& base_diff_filter)
    {
        base_diff_filter_ = &base_diff_filter;
        clear_cache();
    }

    void process_heaps::clear_base_diff_filter()
    {
        base_diff_filter_ = nullptr;
        clear_cache();
    }

    std::experimental::generator<process_heap_entry> process_heaps::entries() const
    {
        if(entry_filters_cache_.empty())
        {
            std::ranges::copy(filter_entries(), std::back_inserter(entry_filters_cache_));
        }

        if(entry_cache_.empty())
        {
            for (auto const& entry : all_entries())
            {
                if(!is_filtered(entry_filters_cache_, entry.second))
                {
                    entry_cache_.emplace_back(entry.second);
                    co_yield entry.second;
                }
            }
        }
        else
        {
            for (auto const& entry : entry_cache_)
            {
                co_yield entry;
            }
        }
    }

    std::experimental::generator<process_heap_entry> process_heaps::free_entries() const
    {
        if(free_entry_filters_cache_.empty())
        {
            std::ranges::copy(filter_free_entries(), std::back_inserter(free_entry_filters_cache_));
        }

        if(free_entry_cache_.empty())
        {
            for (auto const& entry : all_free_entries())
            {
                if(!is_filtered(free_entry_filters_cache_, entry))
                {
                    free_entry_cache_.emplace_back(entry);
                    co_yield entry;
                }
            }
        }
        else
        {
            for (auto const& entry : free_entry_cache_)
            {
                co_yield entry;
            }
        }
    }

    bool process_heaps::is_address_filtered(uint64_t const address, size_units::base_16::bytes const size) const
    {
        if(is_address_filtered(entry_filters_cache_, address, size))
        {
            return true;
        }

        return is_address_filtered(free_entry_filters_cache_, address, size);
    }

    process_heaps_statistics process_heaps::statistics() const
    {
        return process_heaps_statistics{*this, system_module(), statistic_view_options()};
    }

    void process_heaps::clear_cache() const
    {
        entry_filters_cache_.clear();
        free_entry_filters_cache_.clear();
        entry_cache_.clear();
        free_entry_cache_.clear();
    }

    std::vector<heap_subsegment> process_heaps::get_all_nt_heap_lfh_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap) const
    {
        std::vector<heap_subsegment> lfh_data;
        if (auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
        {
            for (auto const& segment : lfh_heap.value().lfh_segments())
            {
                for (auto const& subsegment : segment.subsegments())
                {
                    for (auto const& entry : subsegment.entries())
                    {
                        if (entry.is_busy())
                        {
                            if (auto const [crt_entry, skip_next_entry_value] = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::nt_heap_lfh_entry); crt_entry != nullptr)
                            {
                                if (crt_entry->block_use())
                                {
                                    add_heap_entry(all_entries, process_heap_entry{entry, *crt_entry});
                                }
                            }
                            else
                            {
                                add_heap_entry(all_entries, process_heap_entry{entry});
                            }
                        }
                    }

                    lfh_data.emplace_back(subsegment);
                }
            }
        }

        return lfh_data;
    }

    void process_heaps::get_all_nt_heap_segment_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap, std::vector<heap_subsegment> const& lfh_data) const
    {
        for (auto const& segment : nt_heap.segments())
        {
            for (auto const& entry : segment.entries())
            {
                if (entry.is_busy() && 
                    !std::ranges::any_of(lfh_data, [&entry](heap_subsegment const& subsegment) { return is_lfh_subsegment_in_entry(entry, subsegment); }))
                {
                    if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::nt_heap_segment_entry); crt_entry != nullptr)
                    {
                        if (crt_entry->block_use())
                        {
                            add_heap_entry(all_entries, process_heap_entry{entry, *crt_entry});
                        }
                    }
                    else
                    {
                        add_heap_entry(all_entries, process_heap_entry{entry});
                    }
                }
            }
        }
    }

    void process_heaps::get_all_nt_heap_virtual_entries(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, nt_heap const& nt_heap) const
    {
        for (auto const& virtual_block : nt_heap.heap_virtual_blocks())
        {
            for (auto const& entry : virtual_block.entries())
            {
                if (entry.is_busy())
                {
                    if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::nt_heap_virtual_entry); crt_entry != nullptr)
                    {
                        if (crt_entry->block_use())
                        {
                            add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                        }
                    }
                    else
                    {
                        add_heap_entry(all_entries, process_heap_entry{ entry });
                    }
                }
            }
        }
    }

    void process_heaps::get_all_segment_backend_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const
    {
        for (auto const& segment_context : segment_heap.segment_contexts())
        {
            for (auto const& page : segment_context.pages())
            {
                for (auto const& entry : page.entries())
                {
                    if (entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT)
                    {
                        if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::segment_backend_entry); crt_entry != nullptr)
                        {
                            if (crt_entry->block_use())
                            {
                                add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                            }
                        }
                        else
                        {
                            add_heap_entry(all_entries, process_heap_entry{ entry });
                        }
                    }
                }
            }
        }
    }

    void process_heaps::get_all_segment_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const
    {
        for (auto const& subsegment : segment_heap.vs_context().subsegments())
        {
            for (auto const& entry : subsegment.entries())
            {
                if (!entry.uncommitted_range() && entry.allocated())
                {
                    if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::segment_entry); crt_entry != nullptr)
                    {
                        if (crt_entry->block_use())
                        {
                            add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                        }
                    }
                    else
                    {
                        add_heap_entry(all_entries, process_heap_entry{ entry });
                    }
                }
            }
        }
    }

    void process_heaps::get_all_segment_lfh_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const
    {
        for (auto const& bucket : segment_heap.lfh_context().active_buckets())
        {
            if (bucket.is_enabled())
            {
                for (auto const& affinity_slot : bucket.affinity_slots())
                {
                    for (auto const& subsegment : affinity_slot.subsegments())
                    {
                        for (auto const& entry : subsegment.entries())
                        {
                            if (entry.allocated())
                            {
                                if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::segment_lfh_entry); crt_entry != nullptr)
                                {
                                    if (crt_entry->block_use())
                                    {
                                        add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                                    }
                                }
                                else
                                {
                                    add_heap_entry(all_entries, process_heap_entry{ entry });
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void process_heaps::get_all_segment_large_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, segment_heap const& segment_heap) const
    {
        for (auto const& entry : segment_heap.large_entries())
        {
            if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::segment_large_entry); crt_entry != nullptr)
            {
                if (crt_entry->block_use())
                {
                    add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                }
            }
            else
            {
                add_heap_entry(all_entries, process_heap_entry{ entry });
            }
        }
    }

    void process_heaps::get_all_dph_entities(std::map<uint64_t, process_heap_entry>& all_entries, std::map<uint64_t, crt_entry> const& crt_entries, dph_heap const& heap) const
    {
        for (auto const& entry : heap.busy_entries())
        {
            if (entry.is_allocated())
            {
                if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::dph_entry); crt_entry != nullptr)
                {
                    if (crt_entry->block_use())
                    {
                        add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                    }
                }
                else
                {
                    add_heap_entry(all_entries, process_heap_entry{ entry });
                }
            }
        }
    }

    void process_heaps::get_all_dph_virtual_entities(std::map<uint64_t, process_heap_entry> & all_entries, std::map<uint64_t, crt_entry> const& crt_entries, dph_heap const& heap) const
    {
        for (auto const& entry : heap.virtual_ranges())
        {
            if (entry.is_allocated())
            {
                if (auto const [crt_entry, skip_next_entry_value]  = match_crt_entry(entry.user_address(), entry.user_requested_size(), crt_entries, heap_node_type::dph_virtual_entry); crt_entry != nullptr)
                {
                    if (crt_entry->block_use())
                    {
                        add_heap_entry(all_entries, process_heap_entry{ entry, *crt_entry });
                    }
                }
                else
                {
                    add_heap_entry(all_entries, process_heap_entry{ entry });
                }
            }
        }
    }

    bool process_heaps::is_system_allocation(memory_range const& range) const
    {
        return std::ranges::any_of(system_area_addresses_, [&range](uint64_t const address) { return address >= range.start_range && address < range.end_range; });
    }

    void process_heaps::get_all_virtual_alloc_entities(std::map<uint64_t, process_heap_entry>& all_entries) const
    {
        for (auto const& range : peb().walker().memory_ranges())
        {
            add_heap_entry(all_entries, process_heap_entry{peb(), range});
        }
    }

    std::map<uint64_t, process_heap_entry> process_heaps::all_entries() const
    {
        std::map<uint64_t, process_heap_entry> all_entries;

        crt_heap const crt_heap{ cache(), peb() };
        std::map<uint64_t, crt_entry> crt_entries;
        for (auto const& entry : crt_heap.entries())
        {
            crt_entries.insert(std::make_pair(entry.end_entry_address() - 1, entry));
        }

        get_all_virtual_alloc_entities(all_entries);

        for (uint32_t heap_index = 0; heap_index < peb().number_of_heaps(); ++heap_index)
        {
            if (auto const nt_heap = peb().nt_heap(heap_index); nt_heap.has_value())
            {
                auto const lfh_data{get_all_nt_heap_lfh_entries(all_entries, crt_entries, nt_heap.value())};
                get_all_nt_heap_segment_entries(all_entries, crt_entries, nt_heap.value(), lfh_data);
                get_all_nt_heap_virtual_entries(all_entries, crt_entries, nt_heap.value());
            }
            else if (auto const segment_heap = peb().segment_heap(heap_index); segment_heap.has_value())
            {
                get_all_segment_backend_entities(all_entries, crt_entries, segment_heap.value());
                get_all_segment_entities(all_entries, crt_entries, segment_heap.value());
                get_all_segment_lfh_entities(all_entries, crt_entries, segment_heap.value());
                get_all_segment_large_entities(all_entries, crt_entries, segment_heap.value());
            }
        }

        for (auto const& heap : dph_heap::dph_heaps(cache(), peb()))
        {
            get_all_dph_entities(all_entries, crt_entries, heap);
            get_all_dph_virtual_entities(all_entries, crt_entries, heap);
        }

        return all_entries;
    }

    std::experimental::generator<process_heap_entry> process_heaps::all_free_entries() const
    {
        crt_heap const crt_heap{ cache(), peb() };
        std::vector<crt_entry> crt_entries;
        for (auto const& entry : crt_heap.entries())
        {
            if (!entry.block_use())
            {
                crt_entries.emplace_back(entry);
            }
        }

        for (uint32_t heap_index = 0; heap_index < peb().number_of_heaps(); ++heap_index)
        {
            if (auto const nt_heap = peb().nt_heap(heap_index); nt_heap.has_value())
            {
                std::vector<heap_subsegment> lfh_data;
                if (auto const lfh_heap = nt_heap.value().lfh_heap(); lfh_heap.has_value())
                {
                    for (auto const& segment : lfh_heap.value().lfh_segments())
                    {
                        for (auto const& subsegment : segment.subsegments())
                        {
                            for (auto const& entry : subsegment.entries())
                            {
                                if (entry.is_busy())
                                {
                                    if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.address(), entry.size().count(), crt_entry.user_address()); });
                                        it != crt_entries.end())
                                    {
                                        co_yield process_heap_entry{ entry, *it };
                                    }
                                }
                                else
                                {
                                    co_yield process_heap_entry{ entry };
                                }
                            }

                            lfh_data.emplace_back(subsegment);
                        }
                    }
                }

                for (auto const& segment : nt_heap.value().segments())
                {
                    for (auto const& entry : segment.entries())
                    {
                        if (!std::ranges::any_of(lfh_data, [&entry](heap_subsegment const& subsegment) { return is_lfh_subsegment_in_entry(entry, subsegment); }))
                        {
                            if (entry.is_busy())
                            {
                                if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.address(), entry.size().count(), crt_entry.user_address()); });
                                    it != crt_entries.end())
                                {
                                    co_yield process_heap_entry{ entry, *it };
                                }
                            }
                            else
                            {
                                co_yield process_heap_entry{ entry };
                            }
                        }
                    }
                }

                for (auto const& virtual_block : nt_heap.value().heap_virtual_blocks())
                {
                    for (auto const& entry : virtual_block.entries())
                    {
                        if (entry.is_busy())
                        {
                            if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.address(), entry.size().count(), crt_entry.user_address()); });
                                it != crt_entries.end())
                            {
                                co_yield process_heap_entry{ entry, *it };
                            }
                        }
                        else
                        {
                            co_yield process_heap_entry{ entry };
                        }
                    }
                }
            }
            else if (auto const segment_heap = peb().segment_heap(heap_index); segment_heap.has_value())
            {
                for (auto const& segment_context : segment_heap.value().segment_contexts())
                {
                    for (auto const& page : segment_context.pages())
                    {
                        for (auto const& entry : page.entries())
                        {
                            if (entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT)
                            {
                                if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.block_address(), entry.block_size().count(), crt_entry.user_address()); });
                                    it != crt_entries.end())
                                {
                                    co_yield process_heap_entry{ entry, *it };
                                }
                            }
                            else if (entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_FREE)
                            {
                                co_yield process_heap_entry{ entry };
                            }
                        }
                    }
                }

                for (auto const& subsegment : segment_heap.value().vs_context().subsegments())
                {
                    for (auto const& entry : subsegment.entries())
                    {
                        if (!entry.uncommitted_range() && entry.allocated())
                        {
                            if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.block_address(), entry.block_size(), crt_entry.user_address()); });
                                it != crt_entries.end())
                            {
                                co_yield process_heap_entry{ entry, *it };
                            }
                        }
                        else
                        {
                            co_yield process_heap_entry{ entry };
                        }
                    }
                }

                for (auto const& bucket : segment_heap.value().lfh_context().active_buckets())
                {
                    if (bucket.is_enabled())
                    {
                        for (auto const& affinity_slot : bucket.affinity_slots())
                        {
                            for (auto const& subsegment : affinity_slot.subsegments())
                            {
                                for (auto const& entry : subsegment.entries())
                                {
                                    if (entry.allocated())
                                    {
                                        if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.block_address(), entry.block_size().count(), crt_entry.user_address()); });
                                            it != crt_entries.end())
                                        {
                                            co_yield process_heap_entry{ entry, *it };
                                        }
                                    }
                                    else
                                    {
                                        co_yield process_heap_entry{ entry };
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        for (auto const& heap : dph_heap::dph_heaps(cache(), peb()))
        {
            for (auto const& entry : heap.busy_entries())
            {
                if (entry.is_allocated())
                {
                    if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.virtual_block_address(), entry.virtual_block_size().count(), crt_entry.user_address()); });
                        it != crt_entries.end())
                    {
                        co_yield process_heap_entry{ entry, *it };
                    }
                }
                else
                {
                    co_yield process_heap_entry{ entry };
                }
            }

            for (auto const& entry : heap.virtual_ranges())
            {
                if (entry.is_allocated())
                {
                    if (auto it = std::ranges::find_if(crt_entries, [&entry](auto const& crt_entry) { return contains_address(entry.virtual_block_address(), entry.virtual_block_size().count(), crt_entry.user_address()); });
                        it != crt_entries.end())
                    {
                        co_yield process_heap_entry{ entry, *it };
                    }
                }
                else
                {
                    co_yield process_heap_entry{ entry };
                }
            }
        }
    }

    std::experimental::generator<process_heap_entry> process_heaps::filter_entries() const
    {
        if(base_diff_filter_ != nullptr)
        {
            for (auto const& entry : base_diff_filter_->entries())
            {
                co_yield entry;
            }
        }
    }

    std::experimental::generator<process_heap_entry> process_heaps::filter_free_entries() const
    {
        if(base_diff_filter_ != nullptr)
        {
            for (auto const& entry : base_diff_filter_->free_entries())
            {
                co_yield entry;
            }
        }
    }

    std::tuple<crt_entry const*, bool> process_heaps::match_crt_entry(uint64_t const user_address, size_units::base_16::bytes const size, std::map<uint64_t, crt_entry> const& crt_entries, heap_node_type const node_type) const
    {
        const auto it = crt_entries.lower_bound(user_address);
        if(it != crt_entries.end())
        {
            auto const match = heap_match_utils::does_memory_match_to_range(peb().walker(), it->second.entry_address(), it->second.entry_size(), user_address, size);
            options_->match_result_callback(it->second, user_address, size, match, node_type);
            switch(match)
            {
            case block_range_match_result::block_match:
            case block_range_match_result::block_contains:
                return std::make_tuple(&it->second, false);

            case block_range_match_result::user_contains_block:
            case block_range_match_result::block_partially_contains:
            case block_range_match_result::block_no_match:
                break;
            }
        }

        return std::make_tuple(nullptr, false);
    }

    void process_heaps::add_heap_entry(std::map<uint64_t, process_heap_entry>& entries, process_heap_entry process_heap_entry)
    {
        if(auto const it = entries.lower_bound(process_heap_entry.user_address()); it != entries.end())
        {
            if(does_entry_contain_entry(it->second, process_heap_entry))
            {
                // remove containing entry
                entries.erase(it);
            }
            else if(does_entry_contain_entry(process_heap_entry, it->second))
            {
                // don't add containing entry
                return;
            }
        }

        auto end_user_address = get_last_address(process_heap_entry);
        entries.emplace(std::make_pair(end_user_address, std::move(process_heap_entry)));
    }

    bool process_heaps::does_entry_contain_entry(process_heap_entry const& container_heap_entry, process_heap_entry const& heap_entry)
    {
        return heap_entry.user_address() >= container_heap_entry.user_address() &&
            heap_entry.user_address() + heap_entry.user_requested_size().count() <= container_heap_entry.user_address() + container_heap_entry.user_requested_size().count();
    }

    bool process_heaps::is_filtered(std::vector<process_heap_entry> const& filters, process_heap_entry const& entry)
    {
        return std::ranges::find(filters, entry) != filters.end();
    }

    bool process_heaps::is_address_filtered(std::vector<process_heap_entry> const& filters, uint64_t const address, size_units::base_16::bytes const size)
    {
        return std::ranges::find_if(filters, [&address, &size](process_heap_entry const& entry) { return entry.user_address() == address && entry.user_requested_size() == size; }) != filters.end();
    }

    bool process_heaps::is_lfh_subsegment_in_entry(heap_entry const& entry, heap_subsegment const& subsegment)
    {
        auto const is_internal_entry = (entry.flags() & (heap_entry::FlagBusy | heap_entry::FlagVirtualAlloc)) == (heap_entry::FlagBusy | heap_entry::FlagVirtualAlloc);
        return is_internal_entry && subsegment.entry_start_address() > entry.address() && subsegment.entry_start_address() < entry.address() + entry.size().count();
    }

    bool process_heaps::contains_address(uint64_t const start_address, uint64_t const size, uint64_t const address)
    {
        return address >= start_address && address < start_address + size;
    }
}
