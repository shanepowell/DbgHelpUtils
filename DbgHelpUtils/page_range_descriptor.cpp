#include "page_range_descriptor.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_segment_context.h"
#include "page_range_flags_utils.h"
#include "process_environment_block.h"
#include "segment_heap.h"
#include "segment_heap_utils.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    std::wstring const& page_range_descriptor::symbol_name = common_symbol_names::heap_page_range_descriptor_structure_symbol_name;

    page_range_descriptor::page_range_descriptor(heap_segment_context const& heap, uint64_t const page_range_descriptor_address, size_t const index, uint64_t const heap_page_segment_address)
    : cache_data_{&heap.heap().cache().get_cache<cache_data>()}
    , heap_{&heap}
    , page_range_descriptor_address_{page_range_descriptor_address}
    , index_{index}
    , heap_page_segment_address_{heap_page_segment_address}
    , unit_shift_{heap.unit_shift()}
    , front_padding_size_{get_front_padding_size()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& page_range_descriptor::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& page_range_descriptor::peb() const
    {
        return heap().peb();
    }

    bool page_range_descriptor::extra_present() const
    {
        return stream_utils::get_bit_field_value<uint16_t>(*this, cache_data_->heap_page_range_descriptor_extra_present_field_data, common_symbol_names::heap_page_range_descriptor_extra_present_field_symbol_name) == 0x1;
    }

    size_units::base_16::bytes page_range_descriptor::extra_bytes() const
    {
        return size_units::base_16::bytes{stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_page_range_descriptor_unused_bytes_field_data, common_symbol_names::heap_page_range_descriptor_unused_bytes_field_symbol_name)};
    }

    page_range_flags_utils::page_range_flags page_range_descriptor::range_flags() const
    {
        if(peb().is_x64_target())
        {
            return static_cast<page_range_flags_utils::page_range_flags>(stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_range_flags_field_data, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name));
        }
        if(peb().is_x86_target())
        {
            return static_cast<page_range_flags_utils::page_range_flags>(stream_utils::get_bit_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_range_flags_bits_field_data, common_symbol_names::heap_page_range_descriptor_range_flags_bits_field_symbol_name));
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name);
    }

    uint8_t page_range_descriptor::committed_page_count() const
    {
        if(heap().peb().is_x64_target())
        {
            return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_committed_page_count_field_data_x64, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name);
        }
        if(heap().peb().is_x86_target())
        {
            return stream_utils::get_bit_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_committed_page_count_field_data_x86, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name);
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name);
    }

    uint8_t page_range_descriptor::unit_offset() const
    {
        if(!is_start_of_range())
        {
            return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_unit_size_field_data, common_symbol_names::heap_page_range_descriptor_unit_size_field_symbol_name);
        }

        return 0;
    }

    size_units::base_16::bytes page_range_descriptor::unit_size() const
    {
        if(is_start_of_range())
        {
            return size_units::base_16::bytes{stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_page_range_descriptor_unit_size_field_data, common_symbol_names::heap_page_range_descriptor_unit_size_field_symbol_name)};
        }

        return size_units::base_16::bytes{1};
    }

    uint64_t page_range_descriptor::block_address() const
    {
        return heap_page_segment_address_ + (index_ * (0x01ULL << unit_shift_));
    }

    uint64_t page_range_descriptor::user_address() const
    {
        return block_address() + front_padding_size().value_or(0);
    }

    size_units::base_16::bytes page_range_descriptor::block_size() const
    {
        return size_units::base_16::bytes{(unit_size().count() << unit_shift_)};
    }

    size_units::base_16::bytes page_range_descriptor::user_requested_size() const
    {
        auto const size = (unit_size().count() << unit_shift_);
        auto const extra = extra_bytes().count();

        if(extra >= size)
        {
            return size_units::base_16::bytes{size - front_padding_size().value_or(0)};
        }

        return size_units::base_16::bytes{size - extra - front_padding_size().value_or(0)};
    }

    bool page_range_descriptor::is_start_of_range() const
    {
        auto const flags = range_flags();
        return static_cast<page_range_flags_utils::page_range_flags>(static_cast<uint32_t>(flags) & static_cast<uint32_t>(page_range_flags_utils::page_range_flags::PAGE_RANGE_FLAGS_COMMITTED)) == page_range_flags_utils::page_range_flags::PAGE_RANGE_FLAGS_COMMITTED;
    }

    void page_range_descriptor::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_page_range_descriptor_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);

            data.heap_page_range_descriptor_unused_bytes_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_page_range_descriptor_symbol_type, symbol_name, common_symbol_names::heap_page_range_descriptor_unused_bytes_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            if(heap.peb().is_x64_target())
            {
                data.heap_page_range_descriptor_range_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_page_range_descriptor_symbol_type, symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            }
            data.heap_page_range_descriptor_committed_page_count_field_data_x64 = stream_utils::get_field_type_and_offset_in_type(data.heap_page_range_descriptor_symbol_type, symbol_name, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_page_range_descriptor_unit_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_page_range_descriptor_symbol_type, symbol_name, common_symbol_names::heap_page_range_descriptor_unit_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_page_range_descriptor_extra_present_field_data = data.heap_page_range_descriptor_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_page_range_descriptor_extra_present_field_symbol_name);
            if(heap.peb().is_x86_target())
            {
                data.heap_page_range_descriptor_range_flags_bits_field_data = data.heap_page_range_descriptor_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_page_range_descriptor_range_flags_bits_field_symbol_name);
            }
            data.heap_page_range_descriptor_committed_page_count_field_data_x86 = data.heap_page_range_descriptor_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_page_range_descriptor_committed_page_count_field_symbol_name);
        }
    }

    std::optional<uint64_t> page_range_descriptor::get_front_padding_size() const
    {
        return segment_heap_utils::read_front_padding_size(peb(), block_address(), block_size().count());
    }

    uint64_t page_range_descriptor::get_ust_address() const
    {
        if(!peb().user_stack_db_enabled() || !extra_present())
        {
            return 0;
        }

        return segment_heap_utils::read_ust_address(peb(), heap().stack_trace(), block_address(), block_size().count(), extra_bytes().count());
    }

    std::vector<uint64_t> page_range_descriptor::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }
}
