#include "page_range_descriptor.h"

#include "common_symbol_names.h"
#include "page_range_flags_utils.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& page_range_descriptor::symbol_name = common_symbol_names::heap_page_range_descriptor_structure_symbol_name;

    page_range_descriptor::page_range_descriptor(segment_heap const& heap, uint64_t const page_range_descriptor_address, size_t const index, uint64_t const heap_page_segment_address)
    : heap_{heap}
    , page_range_descriptor_address_{page_range_descriptor_address}
    , index_{index}
    , heap_page_segment_address_{heap_page_segment_address}
    , heap_page_range_descriptor_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& page_range_descriptor::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& page_range_descriptor::peb() const
    {
        return heap().peb();
    }

    bool page_range_descriptor::extra_present() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, common_symbol_names::heap_page_range_descriptor_extra_present_field_symbol_name) == 0x1;
    }

    size_units::base_10::bytes page_range_descriptor::extra_bytes() const
    {
        return size_units::base_10::bytes{stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::heap_page_range_descriptor_unused_bytes_field_symbol_name)};
    }

    page_range_flags_utils::page_range_flags page_range_descriptor::range_flags() const
    {
        if(heap().peb().is_x64_target())
        {
            return static_cast<page_range_flags_utils::page_range_flags>(stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name));
        }
        if(heap().peb().is_x86_target())
        {
            return static_cast<page_range_flags_utils::page_range_flags>(stream_utils::get_bit_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_range_flags_bits_field_symbol_name));
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name);
    }

    uint8_t page_range_descriptor::committed_page_count() const
    {
        if(heap().peb().is_x64_target())
        {
            return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name);
        }
        if(heap().peb().is_x86_target())
        {
            return stream_utils::get_bit_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name);
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name);
    }

    uint8_t page_range_descriptor::unit_offset() const
    {
        if(!is_start_of_range())
        {
            return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_unit_size_field_symbol_name);
        }

        return 0;
    }

    size_units::base_10::bytes page_range_descriptor::unit_size() const
    {
        if(is_start_of_range())
        {
            return size_units::base_10::bytes{stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_page_range_descriptor_unit_size_field_symbol_name)};
        }

        return size_units::base_10::bytes{1};
    }

    uint64_t page_range_descriptor::block_address() const
    {
        return heap_page_segment_address_ + (index_ * get_unit_block_size());
    }

    uint64_t page_range_descriptor::user_address() const
    {
        return block_address() + read_front_padding_size();
    }

    size_units::base_10::bytes page_range_descriptor::block_size() const
    {
        return size_units::base_10::bytes{(unit_size() * get_unit_block_size())};
    }

    size_units::base_10::bytes page_range_descriptor::user_size() const
    {
        auto const size = (unit_size().count() * get_unit_block_size()) - read_front_padding_size();
        auto const extra = extra_bytes().count();

        if(extra >= size)
        {
            return size_units::base_10::bytes{size};
        }

        return size_units::base_10::bytes{size - extra};
    }

    bool page_range_descriptor::is_start_of_range() const
    {
        auto const flags = range_flags();
        return static_cast<page_range_flags_utils::page_range_flags>(static_cast<uint32_t>(flags) & static_cast<uint32_t>(page_range_flags_utils::page_range_flags::PAGE_RANGE_FLAGS_COMMITTED)) == page_range_flags_utils::page_range_flags::PAGE_RANGE_FLAGS_COMMITTED;
    }

    int64_t page_range_descriptor::read_front_padding_size() const
    {
        if(range_flags() != page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT)
        {
            return 0;
        }

        return stream_utils::read_field_value<uint32_t>(walker(), block_address()).value_or(0);
    }

    int64_t page_range_descriptor::get_unit_block_size() const
    {
        if(peb().is_x64_target())
        {
            return 0x10000;
        }
        if(peb().is_x86_target())
        {
            return 0x1000;
        }

        return 0x1000;
    }
}
