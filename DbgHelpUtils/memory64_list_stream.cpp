#include "memory64_list_stream.h"

#include <algorithm>
#include <map>
#include <ranges>

#include "memory_range.h"
#include "mini_dump.h"
#include "range_units.h"

namespace dlg_help_utils
{
    memory64_list_stream::memory64_list_stream(mini_dump const& dump, size_t index)
        : dump_{&dump}
        , memory_list_{get_memory_list(dump, index)}
        , index_{index}
        , found_{memory_list_ != nullptr}
    {
    }

    void const* memory64_list_stream::find_address_range(uint64_t const address, uint64_t const length) const
    {
        if(memory_address_ranges_.empty()) return nullptr;
        if (!range_utils::validate_range(address, length)) return nullptr;

        auto const it = memory_address_ranges_upper_bound(address);
        if(it == memory_address_ranges_.end()) return nullptr;

        if (range_utils::range_contains(it->start_of_memory_range, it->location.DataSize, address, length))
        {
            return static_cast<uint8_t const*>(dump_->rva64(it->location)) + (address - it->start_of_memory_range);
        }

        return nullptr;
    }

    void const* memory64_list_stream::find_any_address_range(const uint64_t address, uint64_t& length) const
    {
        if(memory_address_ranges_.empty()) return nullptr;

        range_utils::limit_range(address, length);
        if (length == 0) return nullptr;

        auto const it = memory_address_ranges_upper_bound(address);
        if(it == memory_address_ranges_.end()) return nullptr;

        if (range_utils::range_union(it->start_of_memory_range, it->location.DataSize, address, length))
        {
            return static_cast<uint8_t const*>(dump_->rva64(it->location)) + (address - it->start_of_memory_range);
        }

        return nullptr;
    }

    std::experimental::generator<memory_range> memory64_list_stream::memory_ranges() const
    {
        for (auto const& range : memory_address_ranges_)
        {
            co_yield {range.start_of_memory_range, range.end_of_memory_range};
        }
    }

    std::vector<memory64_entry>::const_iterator memory64_list_stream::memory_address_ranges_upper_bound(uint64_t const address) const
    {
        return std::upper_bound(memory_address_ranges_.begin(), memory_address_ranges_.end(), address, [](uint64_t const& address, memory64_entry const& entry) { return address < entry.end_of_memory_range; });
    }

    MINIDUMP_MEMORY64_LIST const* memory64_list_stream::get_memory_list(mini_dump const& dump, size_t& index)
    {
        auto const* entry = dump.find_stream_type(Memory64ListStream, index);
        if (entry == nullptr)
        {
            return nullptr;
        }

        return static_cast<MINIDUMP_MEMORY64_LIST const*>(dump.rva32(entry->Location));
    }

    std::vector<memory64_entry> memory64_list_stream::build_memory_address_ranges() const
    {
        std::vector<memory64_entry>  rv;
        if(memory_list_ == nullptr)
        {
            return rv;
        }

        std::map<uint64_t, memory64_entry>  sorted_ranges;
        memory64_entry result{};
        result.location.Rva = memory_list_->BaseRva;
        for (size_t index = 0; index < memory_list_->NumberOfMemoryRanges; ++index)
        {
            auto const& entry{memory_list_->MemoryRanges[index]};
            result.start_of_memory_range = entry.StartOfMemoryRange;
            result.end_of_memory_range = entry.StartOfMemoryRange + entry.DataSize;
            result.location.DataSize = entry.DataSize;
            sorted_ranges.insert(std::make_pair(entry.StartOfMemoryRange, result));
            result.location.Rva += entry.DataSize;
        }

        rv.reserve(static_cast<size_t>(memory_list_->NumberOfMemoryRanges));
        for (const auto& entry : sorted_ranges | std::views::values)
        {
            rv.emplace_back(entry);
        }

        return rv;
    }
}
