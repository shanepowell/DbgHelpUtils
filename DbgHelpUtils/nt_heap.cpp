#include "nt_heap.h"

#include <sstream>
#include "common_symbol_names.h"
#include "gflags_utils.h"
#include "heap_entry.h"
#include "heap_segment.h"
#include "heap_ucr_descriptor.h"
#include "heap_virtual_block.h"
#include "lfh_heap.h"
#include "mini_dump_stack_walk.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"


namespace dlg_help_utils::heap
{
    nt_heap::nt_heap(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const nt_heap_address, gflags_utils::gflags const nt_global_flag)
    : walker_{walker}
    , nt_heap_address_{nt_heap_address}
    , nt_global_flag_{nt_global_flag}
    , heap_symbol_type_{stream_utils::get_type(walker, common_symbol_names::heap_structure_symbol_name)}
    , list_entry_symbol_type_{stream_utils::get_type(walker, common_symbol_names::list_entry_structure_symbol_name)}
    , heap_free_entry_symbol_type_{stream_utils::get_type(walker, common_symbol_names::heap_free_entry_structure_symbol_name)}
    , granularity_{get_granularity(walker)}
    , segment_entry_offset_{get_segment_entry_offset(heap_symbol_type_)}
    , free_entry_free_list_offset_{get_free_entry_free_list_offset(heap_free_entry_symbol_type_)}
    {
        if(auto const encode_flag = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, heap_symbol_type_, common_symbol_names::heap_encode_flag_mask_field_symbol_name, nt_heap_address_); (encode_flag.value_or(0x0) & HeapEncodeFlagMakeEncodingEnabled) == HeapEncodeFlagMakeEncodingEnabled)
        {
            if(auto encoding_data = stream_utils::read_udt_value_in_type(walker, heap_symbol_type_, common_symbol_names::heap_encoding_field_symbol_name, nt_heap_address_); encoding_data.has_value())
            {
                encoding_ = std::move(std::get<0>(encoding_data.value()));
            }
        }
    }

    template <typename T>
    T nt_heap::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(walker_, heap_symbol_type_, field_name, nt_heap_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint64_t nt_heap::get_machine_size_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_machine_size_field_value(*this, heap_symbol_type_, field_name, nt_heap_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint32_t nt_heap::flags() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_flags_field_symbol_name);
    }

    size_units::base_10::bytes nt_heap::reserved() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_total_memory_reserved_field_symbol_name) };
    }

    size_units::base_10::bytes nt_heap::committed() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_total_memory_committed_field_symbol_name) };
    }

    size_units::base_10::bytes nt_heap::uncommitted() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_total_memory_large_ucr_field_symbol_name) };
    }

    size_units::base_10::bytes nt_heap::virtual_blocks() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_total_size_in_virtual_blocks_field_symbol_name) };
    }

    uint32_t nt_heap::total_segments() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_total_segments_field_symbol_name);
    }

    uint32_t nt_heap::total_ucrs() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_total_ucrs_field_symbol_name);
    }

    size_units::base_10::bytes nt_heap::total_free_size() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_total_free_size_field_symbol_name) };
    }

    uint16_t nt_heap::allocator_back_trace_index() const
    {
        return get_field_value<uint16_t>(common_symbol_names::heap_allocator_back_trace_index_field_symbol_name);
    }

    uint8_t nt_heap::front_end_heap_type() const
    {
        return get_field_value<uint8_t>(common_symbol_names::heap_front_end_heap_type_field_symbol_name);
    }

    std::experimental::generator<heap_segment> nt_heap::segments() const
    {
        auto const start = get_field_address(common_symbol_names::heap_segment_list_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::heap_segment_list_flink_field_symbol_name);

        while(flink != start)
        {
            co_yield heap_segment{*this, flink - segment_entry_offset_};

            flink = stream_utils::get_field_pointer(walker_, flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::experimental::generator<heap_ucr_descriptor> nt_heap::uncommitted_ranges() const
    {
        auto const start = get_field_address(common_symbol_names::heap_ucr_list_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::heap_ucr_list_flink_field_symbol_name);

        while(flink != start)
        {
            co_yield heap_ucr_descriptor{*this, flink};
            flink = stream_utils::get_field_pointer(walker_, flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::experimental::generator<heap_virtual_block> nt_heap::heap_virtual_blocks() const
    {
        auto const start = get_field_address(common_symbol_names::heap_virtual_allocated_blocks_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::heap_virtual_allocated_blocks_flink_field_symbol_name);

        while(flink != start)
        {
            co_yield heap_virtual_block{*this, flink};

            flink = stream_utils::get_field_pointer(walker_, flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::experimental::generator<heap_entry> nt_heap::free_entries() const
    {
        auto const start = get_field_address(common_symbol_names::heap_free_lists_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::heap_free_lists_flink_field_symbol_name);

        while(flink != start)
        {
            auto const entry_address = flink - free_entry_free_list_offset_;
            auto buffer = std::make_shared<char[]>(granularity());
            if(auto const* entry_data = walker_.get_process_memory(entry_address, granularity()); entry_data != nullptr)
            {

                decode_heap_entry(entry_data, buffer.get());
                co_yield heap_entry{*this, entry_address, std::move(buffer)};;
            }

            flink = stream_utils::get_field_pointer(walker_, flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::optional<lfh_heap> nt_heap::lfh_heap() const
    {
        if(!is_low_fragment_heap_enabled())
        {
            return std::nullopt;
        }

        auto const address = get_field_pointer(common_symbol_names::heap_front_end_heap_field_symbol_name);
        if(address == 0)
        {
            return std::nullopt;
        }

        return heap::lfh_heap{*this, address};
    }

    bool nt_heap::heap_page_alloc_enabled() const
    {
        return (static_cast<uint32_t>(nt_global_flag_) & static_cast<uint32_t>(gflags_utils::gflags::FLG_HEAP_PAGE_ALLOCS)) == static_cast<uint32_t>(gflags_utils::gflags::FLG_HEAP_PAGE_ALLOCS);
    }

    bool nt_heap::user_stack_db_enabled() const
    {
        return (static_cast<uint32_t>(nt_global_flag_) & static_cast<uint32_t>(gflags_utils::gflags::FLG_USER_STACK_TRACE_DB)) == static_cast<uint32_t>(gflags_utils::gflags::FLG_USER_STACK_TRACE_DB);
    }

    bool nt_heap::decode_heap_entry(void const* src, void* dst) const
    {
        if(is_encoded())
        {
            auto const* encoding = reinterpret_cast<uint8_t const*>(encoding_.get());
            auto * dst_data = static_cast<uint8_t*>(dst);
            auto const* src_data = static_cast<uint8_t const*>(src);
            for(auto const* src_data_end = src_data + granularity(); src_data < src_data_end; ++src_data, ++dst_data, ++encoding)
            {
                *dst_data = *src_data ^ *encoding;
            }
        }
        else
        {
            memcpy(dst, src, granularity());
        }
        return true;
    }

    uint64_t nt_heap::get_field_pointer(std::wstring const& field_name) const
    {
        return stream_utils::get_field_pointer(walker_, nt_heap_address_, heap_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name, field_name);
    }

    uint64_t nt_heap::get_field_address(std::wstring const& field_name) const
    {
        return nt_heap_address_ + stream_utils::get_field_offset(heap_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name, field_name);
    }

    uint32_t nt_heap::get_granularity(stream_stack_dump::mini_dump_stack_walk const& walker)
    {
        auto const heap_entry = stream_utils::get_type(walker, common_symbol_names::heap_entry_structure_symbol_name);
        if(auto const length_data = heap_entry.length(); length_data.has_value())
        {
            return static_cast<uint32_t>(length_data.value());
        }

        throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: symbol " << common_symbol_names::heap_entry_structure_symbol_name << " has no length").str()};
    }

    uint64_t nt_heap::get_segment_entry_offset(dbg_help::symbol_type_info const& heap_symbol_type)
    {
        return stream_utils::get_field_offset(heap_symbol_type, common_symbol_names::heap_entry_structure_symbol_name, common_symbol_names::heap_segment_list_entry_field_symbol_name);
    }

    uint64_t nt_heap::get_free_entry_free_list_offset(dbg_help::symbol_type_info const& heap_free_entry_symbol_type)
    {
        return stream_utils::get_field_offset(heap_free_entry_symbol_type, common_symbol_names::heap_free_entry_structure_symbol_name, common_symbol_names::heap_free_entry_free_list_field_symbol_name);
    }

    void nt_heap::throw_cant_get_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_data(common_symbol_names::heap_structure_symbol_name, field_name);
    }

    void nt_heap::throw_cant_get_field_is_null(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_is_null(common_symbol_names::heap_structure_symbol_name, field_name);
    }
}
