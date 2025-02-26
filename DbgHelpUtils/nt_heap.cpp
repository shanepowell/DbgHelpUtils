#include "nt_heap.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "dph_heap.h"
#include "heap_entry.h"
#include "heap_segment.h"
#include "heap_subsegment.h"
#include "heap_ucr_descriptor.h"
#include "heap_virtual_block.h"
#include "lfh_heap.h"
#include "lfh_segment.h"
#include "list_entry_walker.h"
#include "mini_dump_memory_walker.h"
#include "process_environment_block.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& nt_heap::symbol_name = common_symbol_names::heap_structure_symbol_name;

    nt_heap::nt_heap(cache_manager& cache, process::process_environment_block const& peb, uint64_t const nt_heap_address)
    : cache_manager_{&cache}
    , nt_heap_address_{nt_heap_address}
    , peb_{&peb}
    {
        if(auto const encode_flag = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), cache_data_->heap_encode_flag_mask_field_data, nt_heap_address_); (encode_flag.value_or(0x0) & HeapEncodeFlagMakeEncodingEnabled) == HeapEncodeFlagMakeEncodingEnabled)
        {
            if(auto encoding_data = read_udt_value_in_type(walker(), cache_data_->heap_encoding_field_data, nt_heap_address_); encoding_data.has_value())
            {
                encoding_ = std::move(std::get<0>(encoding_data.value()));
            }
        }
    }

    stream_stack_dump::mini_dump_memory_walker const& nt_heap::walker() const
    {
        return peb().walker();
    }

    uint32_t nt_heap::segment_signature() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_segment_signature_field_data, common_symbol_names::heap_segment_signature_field_symbol_name);
    }

    uint32_t nt_heap::flags() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_flags_field_data, common_symbol_names::heap_flags_field_symbol_name);
    }

    size_units::base_16::bytes nt_heap::reserved() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->heap_total_memory_reserved_field_data, common_symbol_names::heap_total_memory_reserved_field_symbol_name) };
    }

    size_units::base_16::bytes nt_heap::committed() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->heap_total_memory_committed_field_data, common_symbol_names::heap_total_memory_committed_field_symbol_name) };
    }

    size_units::base_16::bytes nt_heap::uncommitted() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->heap_total_memory_large_ucr_field_data, common_symbol_names::heap_total_memory_large_ucr_field_symbol_name) };
    }

    size_units::base_16::bytes nt_heap::virtual_blocks() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->heap_total_size_in_virtual_blocks_field_data, common_symbol_names::heap_total_size_in_virtual_blocks_field_symbol_name) };
    }

    uint32_t nt_heap::total_segments() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_total_segments_field_data, common_symbol_names::heap_total_segments_field_symbol_name);
    }

    uint32_t nt_heap::total_ucrs() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_total_ucrs_field_data, common_symbol_names::heap_total_ucrs_field_symbol_name);
    }

    size_units::base_16::bytes nt_heap::total_free_size() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->heap_total_free_size_field_data, common_symbol_names::heap_total_free_size_field_symbol_name) };
    }

    uint16_t nt_heap::allocator_back_trace_index() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, cache_data_->heap_allocator_back_trace_index_field_data, common_symbol_names::heap_allocator_back_trace_index_field_symbol_name);
    }

    uint8_t nt_heap::front_end_heap_type() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_front_end_heap_type_field_data, common_symbol_names::heap_front_end_heap_type_field_symbol_name);
    }

    std::generator<heap_segment> nt_heap::segments() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{cache(), walker(), stream_utils::get_field_address(*this, cache_data_->heap_segment_list_field_data, common_symbol_names::heap_segment_list_field_symbol_name), heap_segment::symbol_name, common_symbol_names::heap_segment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_segment{*this, entry_address};
        }
    }

    std::generator<heap_ucr_descriptor> nt_heap::uncommitted_ranges() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{cache(), walker(), stream_utils::get_field_address(*this, cache_data_->heap_ucr_list_field_data, common_symbol_names::heap_ucr_list_field_symbol_name), heap_ucr_descriptor::symbol_name, common_symbol_names::heap_ucr_descriptor_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_ucr_descriptor{*this, entry_address};
        }
    }

    std::generator<heap_virtual_block> nt_heap::heap_virtual_blocks() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{cache(), walker(), stream_utils::get_field_address(*this, cache_data_->heap_virtual_allocated_blocks_field_data, common_symbol_names::heap_virtual_allocated_blocks_field_symbol_name), heap_virtual_block::symbol_name, common_symbol_names::heap_virtual_alloc_entry_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_virtual_block{*this, entry_address};
        }
    }

    std::generator<heap_entry> nt_heap::free_entries() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{cache(), walker(), stream_utils::get_field_address(*this, cache_data_->heap_free_lists_field_data, common_symbol_names::heap_free_lists_field_symbol_name), common_symbol_names::heap_free_entry_structure_symbol_name, common_symbol_names::heap_free_entry_free_list_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            auto buffer = std::make_shared<uint8_t[]>(granularity());
            if(auto stream = walker().get_process_memory_stream(entry_address, granularity()); 
                !stream.eof() && stream.read(buffer.get(), granularity()) == granularity())
            {
                decode_heap_entry(buffer.get());
                co_yield heap_entry{*this, entry_address, std::move(buffer)};;
            }
        }
    }

    std::optional<dph_heap> nt_heap::debug_page_heap() const
    {
        if(!peb().heap_page_alloc_enabled())
        {
            return std::nullopt;
        }

        for(auto const& heap : dph_heap::dph_heaps(cache(), peb()))
        {
            if(heap.normal_heap() == nt_heap_address())
            {
                return heap;
            }
        }

        return std::nullopt;
    }

    bool nt_heap::debug_page_heap_available() const
    {
        return debug_page_heap().has_value();
    }

    std::optional<lfh_heap> nt_heap::lfh_heap() const
    {
        if(!is_low_fragment_heap_enabled())
        {
            return std::nullopt;
        }

        auto const address = get_field_pointer(*this, cache_data_->heap_front_end_heap_field_data, common_symbol_names::heap_front_end_heap_field_symbol_name);
        if(address == 0)
        {
            return std::nullopt;
        }

        return heap::lfh_heap{*this, address};
    }

    bool nt_heap::is_process_heap(uint64_t const process_heap_address) const
    {
        auto is_process_heap = nt_heap_address() == process_heap_address;
        if(!is_process_heap)
        {
            auto const dph_heap = debug_page_heap();
            is_process_heap = dph_heap.has_value() && (dph_heap.value().address() - 0x1000) == process_heap_address;
        }

        return is_process_heap;
    }

    void nt_heap::decode_heap_entry(void* buffer) const
    {
        if(is_encoded())
        {
            auto const* encoding = reinterpret_cast<uint8_t const*>(encoding_.get());
            auto * data = static_cast<uint8_t*>(buffer);
            for(auto const* data_end = data + granularity(); data < data_end; ++data, ++encoding)
            {
                *data ^= *encoding;
            }
        }
    }

    nt_heap::cache_data const& nt_heap::setup_globals() const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.heap_symbol_type = stream_utils::get_type(walker(), symbol_name);
            data.granularity = static_cast<uint32_t>(stream_utils::get_type_length(stream_utils::get_type(walker(), heap_entry::symbol_name), heap_entry::symbol_name));

            data.heap_encode_flag_mask_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_encode_flag_mask_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_signature_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_segment_signature_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_memory_reserved_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_memory_reserved_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_memory_committed_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_memory_committed_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_memory_large_ucr_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_memory_large_ucr_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_size_in_virtual_blocks_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_size_in_virtual_blocks_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_segments_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_segments_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_ucrs_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_ucrs_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_total_free_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_total_free_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_allocator_back_trace_index_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_allocator_back_trace_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_front_end_heap_type_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_front_end_heap_type_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_encoding_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_encoding_field_symbol_name, dbg_help::sym_tag_enum::UDT);

            data.heap_front_end_heap_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_symbol_type, symbol_name, common_symbol_names::heap_front_end_heap_field_symbol_name, dbg_help::sym_tag_enum::PointerType);

            data.heap_segment_list_field_data = data.heap_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_segment_list_field_symbol_name);
            data.heap_ucr_list_field_data = data.heap_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_ucr_list_field_symbol_name);
            data.heap_virtual_allocated_blocks_field_data = data.heap_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_virtual_allocated_blocks_field_symbol_name);
            data.heap_free_lists_field_data = data.heap_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_free_lists_field_symbol_name);

            heap_segment::setup_globals(*this);
            heap_ucr_descriptor::setup_globals(*this);
            heap_virtual_block::setup_globals(*this);
            lfh_heap::setup_globals(*this);
            lfh_segment::setup_globals(*this);
            heap_subsegment::setup_globals(*this);
            heap_entry::setup_globals(*this);
        }

        return cache().get_cache<cache_data>();
    }
}
