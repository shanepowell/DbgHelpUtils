#include "memory64_list_stream.h"

#include "mini_dump.h"
#include "range_units.h"

namespace dlg_help_utils
{
    memory64_list_stream::memory64_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(Memory64ListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        memory_list_ = static_cast<MINIDUMP_MEMORY64_LIST const*>(dump.rva32(entry->Location));
        found_ = true;
    }

    std::experimental::generator<memory64_entry> memory64_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        // ReSharper disable once CppInitializedValueIsAlwaysRewritten
        memory64_entry result{};
        result.location.Rva = memory_list_->BaseRva;
        for (size_t index = 0; index < memory_list_->NumberOfMemoryRanges; ++index)
        {
            auto const& entry{memory_list_->MemoryRanges[index]};
            // ReSharper disable once CppAssignedValueIsNeverUsed
            result.start_of_memory_range = entry.StartOfMemoryRange;
            // ReSharper disable once CppAssignedValueIsNeverUsed
            result.location.DataSize = entry.DataSize;
            co_yield result;
            result.location.Rva += entry.DataSize;
        }
    }

    void const* memory64_list_stream::find_address_range(uint64_t const address, uint64_t const length) const
    {
        if (!range_utils::validate_range(address, length)) return nullptr;

        for (auto const& entry : list())
        {
            if (range_utils::range_contains(entry.start_of_memory_range, entry.location.DataSize, address, length))
            {
                return static_cast<uint8_t const*>(dump_.rva64(entry.location)) + (address - entry.start_of_memory_range
                );
            }
        }

        return nullptr;
    }

    void const* memory64_list_stream::find_any_address_range(const uint64_t address, uint64_t& length) const
    {
        range_utils::limit_range(address, length);
        if (length == 0) return nullptr;

        for (auto const& entry : list())
        {
            if (range_utils::range_union(entry.start_of_memory_range, entry.location.DataSize, address, length))
            {
                return static_cast<uint8_t const*>(dump_.rva64(entry.location)) + (address - entry.start_of_memory_range
                );
            }
        }

        return nullptr;
    }
}
