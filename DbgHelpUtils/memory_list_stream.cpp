#include "memory_list_stream.h"

#include "mini_dump.h"
#include "range_units.h"

namespace dlg_help_utils
{
    memory_list_stream::memory_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(MemoryListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        memory_list_ = static_cast<MINIDUMP_MEMORY_LIST const*>(dump.rva32(entry->Location));
        found_ = true;
    }

    std::experimental::generator<MINIDUMP_MEMORY_DESCRIPTOR const*> memory_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        for (size_t index = 0; index < memory_list_->NumberOfMemoryRanges; ++index)
        {
            // ReSharper disable once CppAwaiterTypeIsNotClass
            co_yield &memory_list_->MemoryRanges[index];
        }
    }

    void const* memory_list_stream::find_address_range(uint64_t const address, uint64_t const length) const
    {
        if (!range_utils::validate_range(address, length)) return nullptr;

        for (auto const* entry : list())
        {
            if (range_utils::range_contains(entry->StartOfMemoryRange, entry->Memory.DataSize, address, length))
            {
                return static_cast<uint8_t const*>(dump_.rva32(entry->Memory)) + (address - entry->StartOfMemoryRange);
            }
        }

        return nullptr;
    }

    void const* memory_list_stream::find_any_address_range(uint64_t const address, uint64_t& length) const
    {
        range_utils::limit_range(address, length);
        if (length == 0) return nullptr;

        for (auto const* entry : list())
        {
            if (range_utils::range_union(entry->StartOfMemoryRange, entry->Memory.DataSize, address, length))
            {
                return static_cast<uint8_t const*>(dump_.rva32(entry->Memory)) + (address - entry->StartOfMemoryRange);
            }
        }

        return nullptr;
    }
}
