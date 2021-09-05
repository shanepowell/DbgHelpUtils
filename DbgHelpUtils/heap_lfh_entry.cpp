#include "heap_lfh_entry.h"

#include "heap_lfh_context.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    heap_lfh_entry::heap_lfh_entry(heap_lfh_context const& heap, uint64_t const heap_lfh_entry_address, size_units::base_10::bytes const block_size, bool const allocated, bool const has_unused_bytes)
    : heap_{heap}
    , heap_lfh_entry_address_{heap_lfh_entry_address}
    , block_size_{block_size}
    , allocated_{allocated}
    , has_unused_bytes_{has_unused_bytes}
    , unused_bytes_{get_unused_bytes()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_lfh_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_entry::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_lfh_entry::user_address() const
    {
        return heap_lfh_entry_address() + read_front_padding_size();
    }

    size_units::base_10::bytes heap_lfh_entry::user_size() const
    {
        return size_units::base_10::bytes{block_size().count() - unused_bytes_.count() - read_front_padding_size()};
    }

    size_units::base_10::bytes heap_lfh_entry::get_unused_bytes() const
    {
        if(!has_unused_bytes() || !allocated())
        {
            return size_units::base_10::bytes{0};
        }

        auto const unused_bytes_address = heap_lfh_entry_address() + block_size().count() - 0x02;
        auto const unused_bytes_value = stream_utils::read_field_value<uint16_t>(walker(), unused_bytes_address);
        if(!unused_bytes_value.has_value())
        {
            return size_units::base_10::bytes{0};
        }
        if((unused_bytes_value.value() & 0x8000) == 0x8000)
        {
            // high bit set means 1 byte length...
            return size_units::base_10::bytes{1};
        }

        return size_units::base_10::bytes{unused_bytes_value.value()};
    }

    int64_t heap_lfh_entry::read_front_padding_size() const
    {
        if(!allocated())
        {
            return 0;
        }

        return stream_utils::read_field_value<uint32_t>(walker(), heap_lfh_entry_address()).value_or(0);
    }
}
