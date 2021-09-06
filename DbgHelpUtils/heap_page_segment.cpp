#include "heap_page_segment.h"

#include "common_symbol_names.h"
#include "heap_segment_context.h"
#include "page_range_descriptor.h"
#include "process_environment_block.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_page_segment::symbol_name = common_symbol_names::heap_page_segment_structure_symbol_name;

    heap_page_segment::heap_page_segment(heap_segment_context const& heap, uint64_t const heap_page_segment_address, uint64_t const heap_segment_context_address)
    : heap_{heap}
    , heap_page_segment_address_{heap_page_segment_address}
    , heap_segment_context_address_{heap_segment_context_address}
    , heap_page_segment_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_page_range_descriptor_length_{stream_utils::get_type_length(stream_utils::get_type(walker(), page_range_descriptor::symbol_name), page_range_descriptor::symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_page_segment::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_page_segment::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_page_segment::signature() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_page_segment_signature_field_symbol_name);
    }

    bool heap_page_segment::is_signature_valid() const
    {
        auto const s = signature();
        auto const part = heap_page_segment_address() ^ heap_segment_context_address_ ^ heap().heap_key();
        if(peb().is_x64_target())
        {
            return s == (part ^ 0xA2E64EADA2E64EADULL);
        }
        if(peb().is_x86_target())
        {
            return s == (part ^ 0xA2E64EADUL);
        }
        return false;
    }

    std::experimental::generator<page_range_descriptor> heap_page_segment::all_entries() const
    {
        if(auto const array_count = desc_array_array_field_symbol_type_.array_count(); array_count.has_value())
        {
            auto array_field_address = heap_page_segment_address() + heap_seg_context_array_field_offset_;
            for(size_t index = 0; index < array_count.value(); ++index, array_field_address += heap_page_range_descriptor_length_)
            {
                co_yield page_range_descriptor{heap(), array_field_address, index, heap_page_segment_address()};
            }
        }
    }

    std::experimental::generator<page_range_descriptor> heap_page_segment::entries() const
    {
        if(auto const array_count = desc_array_array_field_symbol_type_.array_count(); array_count.has_value())
        {
            auto array_field_address = heap_page_segment_address() + heap_seg_context_array_field_offset_;
            for(size_t index = 0; index < array_count.value(); ++index, array_field_address += heap_page_range_descriptor_length_)
            {
                if(page_range_descriptor const entry{heap(), array_field_address, index, heap_page_segment_address()};
                    entry.is_start_of_range())
                {
                    co_yield entry;
                }
            }
        }
    }

    std::pair<dbg_help::symbol_type_info, uint64_t> heap_page_segment::get_desc_array_field_data() const
    {
        const auto desc_array = stream_utils::find_field_type_and_offset_in_type(heap_page_segment_symbol_type_, common_symbol_names::heap_page_segment_desc_array_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
        if(!desc_array.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_page_segment_desc_array_field_symbol_name);
        }
        return desc_array.value();
    }

    dbg_help::symbol_type_info heap_page_segment::get_desc_array_field_symbol_type() const
    {
        return std::get<0>(get_desc_array_field_data());
    }

    uint64_t heap_page_segment::get_desc_array_field_offset() const
    {
        return std::get<1>(get_desc_array_field_data());
    }
}
