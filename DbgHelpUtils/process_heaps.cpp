#include "process_heaps.h"

#include "crt_entry.h"
#include "crt_heap.h"
#include "dph_entry.h"
#include "dph_heap.h"
#include "heap_entry.h"
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
#include "nt_heap.h"
#include "page_range_descriptor.h"
#include "process_heap_entry.h"
#include "segment_heap.h"

namespace dlg_help_utils::heap
{
    process_heaps::process_heaps(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine)
    : peb_{ mini_dump, symbol_engine }
    {
    }

    std::experimental::generator<process_heap_entry> process_heaps::entries() const
    {
        crt_heap const crt_heap{ peb_ };
        std::map<uint64_t, crt_entry> crt_entries;
        for (auto const& entry : crt_heap.entries())
        {
            crt_entries.insert(std::make_pair(entry.entry_address(), entry));
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
                                    if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                                    {
                                        if (it->second.block_use())
                                        {
                                            co_yield process_heap_entry{ entry, it->second };
                                        }
                                    }
                                    else
                                    {
                                        co_yield process_heap_entry{ entry };
                                    }
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
                        if (entry.is_busy() && !std::ranges::any_of(lfh_data, [&entry](heap_subsegment const& subsegment) { return is_lfh_subsegment_in_entry(entry, subsegment); }))
                        {
                            if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                            {
                                if (it->second.block_use())
                                {
                                    co_yield process_heap_entry{ entry, it->second };
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
                            if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                            {
                                if (it->second.block_use())
                                {
                                    co_yield process_heap_entry{ entry, it->second };
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
                                if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                                {
                                    if (it->second.block_use())
                                    {
                                        co_yield process_heap_entry{ entry, it->second };
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

                for (auto const& subsegment : segment_heap.value().vs_context().subsegments())
                {
                    for (auto const& entry : subsegment.entries())
                    {
                        if (!entry.uncommitted_range() && entry.allocated())
                        {
                            if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                            {
                                if (it->second.block_use())
                                {
                                    co_yield process_heap_entry{ entry, it->second };
                                }
                            }
                            else
                            {
                                co_yield process_heap_entry{ entry };
                            }
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
                                        if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                                        {
                                            if (it->second.block_use())
                                            {
                                                co_yield process_heap_entry{ entry, it->second };
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

                for (auto const& entry : segment_heap.value().large_entries())
                {
                    if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                    {
                        if (it->second.block_use())
                        {
                            co_yield process_heap_entry{ entry, it->second };
                        }
                    }
                    else
                    {
                        co_yield process_heap_entry{ entry };
                    }
                }
            }
        }

        for (auto const& heap : dph_heap::dph_heaps(peb()))
        {
            for (auto const& entry : heap.busy_entries())
            {
                if (entry.is_allocated())
                {
                    if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                    {
                        if (it->second.block_use())
                        {
                            co_yield process_heap_entry{ entry, it->second };
                        }
                    }
                    else
                    {
                        co_yield process_heap_entry{ entry };
                    }
                }
            }

            for (auto const& entry : heap.virtual_ranges())
            {
                if (entry.is_allocated())
                {
                    if (auto const it = crt_entries.find(entry.user_address()); it != crt_entries.end())
                    {
                        if (it->second.block_use())
                        {
                            co_yield process_heap_entry{ entry, it->second };
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

    std::experimental::generator<process_heap_entry> process_heaps::free_entries() const
    {
        crt_heap const crt_heap{ peb_ };
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

        for (auto const& heap : dph_heap::dph_heaps(peb()))
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

    bool process_heaps::is_lfh_subsegment_in_entry(heap_entry const& entry, heap_subsegment const& subsegment)
    {
        return subsegment.entry_start_address() > entry.address() && subsegment.entry_start_address() < entry.address() + entry.size().count();
    }

    bool process_heaps::contains_address(uint64_t const start_address, uint64_t const size, uint64_t const address)
    {
        return address >= start_address && address < start_address + size;
    }
}
