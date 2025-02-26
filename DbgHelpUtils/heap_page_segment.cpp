#include "heap_page_segment.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_segment_context.h"
#include "page_range_descriptor.h"
#include "process_environment_block.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_page_segment::symbol_name = common_symbol_names::heap_page_segment_structure_symbol_name;

    heap_page_segment::heap_page_segment(heap_segment_context const& heap, uint64_t const heap_page_segment_address, uint64_t const heap_segment_context_address)
    : cache_data_{&heap.heap().cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_page_segment_address_{heap_page_segment_address}
    , heap_segment_context_address_{heap_segment_context_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_page_segment::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_page_segment::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_page_segment::signature() const
    {
        return get_machine_size_field_value(*this, cache_data_->heap_page_segment_signature_field_data, common_symbol_names::heap_page_segment_signature_field_symbol_name);
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

    std::generator<page_range_descriptor> heap_page_segment::all_entries() const
    {
        if(auto const array_count = cache_data_->desc_array_array_field_symbol_type.array_count(); array_count.has_value())
        {
            auto array_field_address = heap_page_segment_address() + cache_data_->heap_seg_context_array_field_offset;
            for(size_t index = 0; index < array_count.value(); ++index, array_field_address += cache_data_->heap_page_range_descriptor_length)
            {
                co_yield page_range_descriptor{heap(), array_field_address, index, heap_page_segment_address()};
            }
        }
    }

    std::generator<page_range_descriptor> heap_page_segment::entries() const
    {
        if(auto const array_count = cache_data_->desc_array_array_field_symbol_type.array_count(); array_count.has_value())
        {
            auto array_field_address = heap_page_segment_address() + cache_data_->heap_seg_context_array_field_offset;
            for(size_t index = 0; index < array_count.value(); ++index, array_field_address += cache_data_->heap_page_range_descriptor_length)
            {
                if(page_range_descriptor const entry{heap(), array_field_address, index, heap_page_segment_address()};
                    entry.is_start_of_range())
                {
                    co_yield entry;
                }
            }
        }
    }

    void heap_page_segment::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_page_segment_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_page_range_descriptor_length = stream_utils::get_type_length(stream_utils::get_type(heap.walker(), page_range_descriptor::symbol_name), page_range_descriptor::symbol_name);
            auto const type_data = stream_utils::get_field_type_and_offset_in_type(data.heap_page_segment_symbol_type, symbol_name, common_symbol_names::heap_page_segment_desc_array_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
            data.desc_array_array_field_symbol_type = type_data.type;
            data.heap_seg_context_array_field_offset = type_data.field_offset;
            data.heap_page_segment_signature_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_page_segment_symbol_type, symbol_name, common_symbol_names::heap_page_segment_signature_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
        }
    }
}
