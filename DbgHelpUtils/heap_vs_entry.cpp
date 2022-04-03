#include "heap_vs_entry.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "segment_heap.h"
#include "segment_heap_utils.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    std::wstring const& heap_vs_entry::symbol_name = common_symbol_names::heap_vs_chunk_header_structure_symbol_name;

    heap_vs_entry::heap_vs_entry(segment_heap const& heap, uint64_t const heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer, uint16_t const previous_size)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_vs_entry_address_{heap_vs_entry_address}
    , buffer_{std::move(buffer)}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , is_valid_{get_is_valid(previous_size)}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_vs_entry::heap_vs_entry(segment_heap const& heap, uint64_t const heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_vs_entry_address_{heap_vs_entry_address}
    , buffer_{std::move(buffer)}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_vs_entry::heap_vs_entry(segment_heap const& heap, uint64_t const heap_vs_entry_address, uint64_t const uncommitted_size)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_vs_entry_address_{heap_vs_entry_address}
    , size_{uncommitted_size}
    , uncommitted_range_{true}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_vs_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_vs_entry::peb() const
    {
        return heap().peb();
    }

    uint16_t heap_vs_entry::memory_cost() const
    {
        validate_buffer();
        return static_cast<uint16_t>(stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_sizes_memory_cost_field_data, common_symbol_names::heap_vs_chunk_header_sizes_memory_cost_field_symbol_name, buffer_.get()));
    }

    bool heap_vs_entry::allocated() const
    {
        validate_buffer();
        return stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_sizes_allocated_field_data, common_symbol_names::heap_vs_chunk_header_sizes_allocated_field_symbol_name, buffer_.get()) != 0;
    }

    uint8_t heap_vs_entry::segment_page_offset() const
    {
        validate_buffer();
        return static_cast<uint8_t>(stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_encoded_segment_page_offset_field_data, common_symbol_names::heap_vs_chunk_header_encoded_segment_page_offset_field_symbol_name, buffer_.get()));
    }

    bool heap_vs_entry::has_unused_bytes() const
    {
        validate_buffer();
        return stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_unused_bytes_field_data, common_symbol_names::heap_vs_chunk_header_unused_bytes_field_symbol_name, buffer_.get()) != 0x0;
    }

    size_units::base_16::bytes heap_vs_entry::unused_bytes() const
    {
        if(!has_unused_bytes() || !allocated())
        {
            return size_units::base_16::bytes{0};
        }

        return size_units::base_16::bytes{segment_heap_utils::read_extra_data(peb(), heap().stack_trace(), block_address(), block_size()).unused_bytes};
    }

    bool heap_vs_entry::skip_during_walk() const
    {
        validate_buffer();
        return stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_skip_during_walk_field_data, common_symbol_names::heap_vs_chunk_header_skip_during_walk_field_symbol_name, buffer_.get()) != 0x0;
    }

    uint32_t heap_vs_entry::spare() const
    {
        validate_buffer();
        return stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_spare_field_data, common_symbol_names::heap_vs_chunk_header_spare_field_symbol_name, buffer_.get());
    }

    uint32_t heap_vs_entry::allocated_chunk_bits() const
    {
        validate_buffer();
        return stream_utils::get_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_allocated_chunk_bits_field_data, common_symbol_names::heap_vs_chunk_header_allocated_chunk_bits_field_symbol_name, buffer_.get());
    }

    uint64_t heap_vs_entry::block_address() const
    {
        return heap_vs_entry_address() + cache_data_.heap_vs_chunk_header_length;
    }

    uint64_t heap_vs_entry::block_size() const
    {
        return size().count() - cache_data_.heap_vs_chunk_header_length;
    }

    uint64_t heap_vs_entry::user_address() const
    {
        return block_address();
    }

    size_units::base_16::bytes heap_vs_entry::user_requested_size() const
    {
        auto requested_user_size = block_size();

        if(has_unused_bytes())
        {
            if(auto const unused_bytes_amount = static_cast<uint64_t>(unused_bytes().count());
                requested_user_size >= unused_bytes_amount)
            {
                requested_user_size -= unused_bytes_amount;
            }
        }

        return size_units::base_16::bytes{requested_user_size};
    }

    uint16_t heap_vs_entry::raw_size() const
    {
        validate_buffer();
        return static_cast<uint16_t>(stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_sizes_unsafe_size_field_data, common_symbol_names::heap_vs_chunk_header_sizes_unsafe_size_field_symbol_name, buffer_.get()));
    }

    void heap_vs_entry::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_vs_chunk_header_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_vs_chunk_header_length = stream_utils::get_type_length(data.heap_vs_chunk_header_symbol_type, symbol_name);

            data.heap_vs_chunk_header_allocated_chunk_bits_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_vs_chunk_header_symbol_type, symbol_name, common_symbol_names::heap_vs_chunk_header_allocated_chunk_bits_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_vs_chunk_header_sizes_memory_cost_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_sizes_memory_cost_field_symbol_name);
            data.heap_vs_chunk_header_sizes_allocated_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_sizes_allocated_field_symbol_name);
            data.heap_vs_chunk_header_encoded_segment_page_offset_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_encoded_segment_page_offset_field_symbol_name);
            data.heap_vs_chunk_header_unused_bytes_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_unused_bytes_field_symbol_name);
            data.heap_vs_chunk_header_skip_during_walk_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_skip_during_walk_field_symbol_name);
            data.heap_vs_chunk_header_spare_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_spare_field_symbol_name);
            data.heap_vs_chunk_header_sizes_unsafe_size_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_sizes_unsafe_size_field_symbol_name);
            data.heap_vs_chunk_header_sizes_unsafe_prev_size_field_data = data.heap_vs_chunk_header_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_vs_chunk_header_sizes_unsafe_prev_size_field_symbol_name);
        }
    }

    size_units::base_16::bytes heap_vs_entry::get_size() const
    {
        return size_units::base_16::bytes{raw_size() << heap().unit_shift_amount()};
    }

    size_units::base_16::bytes heap_vs_entry::get_previous_size() const
    {
        return size_units::base_16::bytes{static_cast<uint64_t>(get_previous_size_raw()) << heap().unit_shift_amount()};
    }

    uint16_t heap_vs_entry::get_previous_size_raw() const
    {
        validate_buffer();
        return static_cast<uint16_t>(stream_utils::get_bit_field_value_from_buffer<uint32_t, heap_vs_entry>(cache_data_.heap_vs_chunk_header_sizes_unsafe_prev_size_field_data, common_symbol_names::heap_vs_chunk_header_sizes_unsafe_prev_size_field_symbol_name, buffer_.get()));
    }

    bool heap_vs_entry::get_is_valid(uint16_t const previous_size) const
    {
        return get_previous_size_raw() == previous_size;
    }

    uint64_t heap_vs_entry::get_ust_address() const
    {
        if(!peb().user_stack_db_enabled() || !allocated() || !has_unused_bytes())
        {
            return 0;
        }

        return segment_heap_utils::read_extra_data(peb(), heap().stack_trace(), block_address(), block_size()).ust_address;
    }

    std::vector<uint64_t> heap_vs_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }

    void heap_vs_entry::validate_buffer() const
    {
        if(!buffer_)
        {
            throw exceptions::wide_runtime_error{L"heap vs entry buffer is null"s};
        }
    }
}
