#include "memory_list_stream.h"

#include <algorithm>
#include <map>
#include <ranges>

#include "memory_range.h"
#include "mini_dump.h"
#include "range_units.h"

namespace dlg_help_utils
{
    memory_list_stream::memory_list_stream(mini_dump const& dump, size_t index)
        : dump_{&dump}
        , memory_list_{get_memory_list(dump, index)}
        , index_{index}
        , found_{memory_list_ != nullptr}
    {
    }

    void const* memory_list_stream::find_address_range(uint64_t const address, uint64_t const length) const
    {
        if(memory_address_ranges_.empty()) return nullptr;
        if (!range_utils::validate_range(address, length)) return nullptr;

        auto const it = memory_address_ranges_upper_bound(address);
        if(it == memory_address_ranges_.end()) return nullptr;

        if (auto const* entry = *it;
            range_utils::range_contains(entry->StartOfMemoryRange, entry->Memory.DataSize, address, length))
        {
            return static_cast<uint8_t const*>(dump_->rva32(entry->Memory)) + (address - entry->StartOfMemoryRange);
        }

        return nullptr;
    }

    void const* memory_list_stream::find_any_address_range(uint64_t const address, uint64_t& length) const
    {
        if(memory_address_ranges_.empty()) return nullptr;
        range_utils::limit_range(address, length);
        if (length == 0) return nullptr;

        auto const it = memory_address_ranges_upper_bound(address);
        if(it == memory_address_ranges_.end()) return nullptr;

        if (auto const* entry = *it;
            range_utils::range_union(entry->StartOfMemoryRange, entry->Memory.DataSize, address, length))
        {
            return static_cast<uint8_t const*>(dump_->rva32(entry->Memory)) + (address - entry->StartOfMemoryRange);
        }

        return nullptr;
    }

    std::generator<memory_range> memory_list_stream::memory_ranges() const
    {
        for (auto const* range : memory_address_ranges_)
        {
            co_yield {.start_range= range->StartOfMemoryRange, .end_range= range->StartOfMemoryRange + range->Memory.DataSize};
        }
    }

    std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*>::const_iterator memory_list_stream::memory_address_ranges_upper_bound(uint64_t const address) const
    {
        MINIDUMP_MEMORY_DESCRIPTOR entry{.StartOfMemoryRange = address, .Memory = {}};
        return std::ranges::upper_bound(memory_address_ranges_, 
            &entry,
            [](MINIDUMP_MEMORY_DESCRIPTOR const* address, MINIDUMP_MEMORY_DESCRIPTOR const* entry) { return address->StartOfMemoryRange < entry->StartOfMemoryRange + entry->Memory.DataSize; });
    }

    MINIDUMP_MEMORY_LIST const* memory_list_stream::get_memory_list(mini_dump const& dump, size_t& index)
    {
        auto const* entry = dump.find_stream_type(MemoryListStream, index);
        if (entry == nullptr)
        {
            return nullptr;
        }

        return static_cast<MINIDUMP_MEMORY_LIST const*>(dump.rva32(entry->Location));
    }

    std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> memory_list_stream::build_memory_address_ranges() const
    {
        std::vector<MINIDUMP_MEMORY_DESCRIPTOR const*> rv;
        if(memory_list_ == nullptr)
        {
            return rv;
        }

        std::map<uint64_t, MINIDUMP_MEMORY_DESCRIPTOR const*> sorted_ranges;
        for (size_t index = 0; index < memory_list_->NumberOfMemoryRanges; ++index)
        {
            auto const& entry = memory_list_->MemoryRanges[index];
            sorted_ranges.insert(std::make_pair(entry.StartOfMemoryRange, &entry));
        }

        rv.reserve(memory_list_->NumberOfMemoryRanges);
        for (const auto& entry : sorted_ranges | std::views::values)
        {
            rv.emplace_back(entry);
        }

        return rv;
    }
}
