#include "segment_heap.h"

#include "common_symbol_names.h"
#include "dph_heap.h"
#include "heap_lfh_context.h"
#include "heap_segment_context.h"
#include "heap_vs_context.h"
#include "large_alloc_entry.h"
#include "rtl_rb_tree_walker.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    std::wstring const& segment_heap::symbol_name = common_symbol_names::segment_heap_structure_symbol_name;

    segment_heap::segment_heap(process::process_environment_block const& peb, uint64_t const segment_heap_address)
    : segment_heap_address_{segment_heap_address}
    , peb_{peb}
    , segment_heap_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_seg_context_symbol_type_{stream_utils::get_type(walker(), heap_segment_context::symbol_name)}
    , heap_seg_context_symbol_length_{stream_utils::get_type_length(heap_seg_context_symbol_type_, heap_segment_context::symbol_name)}
    , heap_seg_context_array_field_symbol_type_{get_heap_seg_context_array_field_symbol_type()}
    , heap_seg_context_array_field_offset_{get_heap_seg_context_array_field_offset()}
    , heap_key_{get_heap_key()}
    , lfh_heap_key_{get_lfh_heap_key()}
    , heap_vs_context_offset_{stream_utils::get_field_offset(segment_heap_symbol_type_, symbol_name, common_symbol_names::segment_heap_vs_context_field_symbol_name)}
    , heap_lfh_context_offset_{stream_utils::get_field_offset(segment_heap_symbol_type_, symbol_name, common_symbol_names::segment_heap_lfh_context_field_symbol_name)}
    , unit_shift_amount_{get_unit_shift_amount()}
    {
    }

    std::pair<dbg_help::symbol_type_info, uint64_t> segment_heap::get_heap_seg_context_array_data() const
    {
        const auto seg_context_array = stream_utils::find_field_type_and_offset_in_type(segment_heap_symbol_type_, common_symbol_names::segment_heap_segment_contexts_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
        if(!seg_context_array.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::segment_heap_seg_context_segment_count_field_symbol_name);
        }
        return seg_context_array.value();
    }

    dbg_help::symbol_type_info segment_heap::get_heap_seg_context_array_field_symbol_type() const
    {
        return std::get<0>(get_heap_seg_context_array_data());
    }

    uint64_t segment_heap::get_heap_seg_context_array_field_offset() const
    {
        return std::get<1>(get_heap_seg_context_array_data());
    }

    template <typename T>
    T segment_heap::get_heap_key_value(std::pair<uint64_t, uint64_t> const& address_and_size, uint64_t const data_offset) const
    {
        if(std::get<1>(address_and_size) < data_offset + sizeof(T))
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        }

        auto const heap_key = stream_utils::read_field_value<T>(walker(), std::get<0>(address_and_size) + data_offset);
        if(!heap_key.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        }

        return heap_key.value();
    }

    uint64_t segment_heap::get_heap_key() const
    {
        auto const address_and_size = stream_utils::get_static_variable_address_and_size(walker(), common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        if(!address_and_size.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        }

        if(peb().is_x64_target())
        {
            return get_heap_key_value<uint64_t>(address_and_size.value(), 0);
        }

        if(peb().is_x86_target())
        {
            return get_heap_key_value<uint32_t>(address_and_size.value(), 0);
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
    }

    uint32_t segment_heap::get_lfh_heap_key() const
    {
        auto const address_and_size = stream_utils::get_static_variable_address_and_size(walker(), common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        if(!address_and_size.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
        }

        if(peb().is_x64_target())
        {
            return get_heap_key_value<uint32_t>(address_and_size.value(), sizeof uint64_t);
        }

        if(peb().is_x86_target())
        {
            return get_heap_key_value<uint32_t>(address_and_size.value(), sizeof uint32_t);
        }

        stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::rtl_p_hp_heap_globals_global_symbol_name);
    }

    uint64_t segment_heap::get_unit_shift_amount() const
    {
        if(peb().is_x64_target())
        {
            return 4;
        }

        if(peb().is_x86_target())
        {
            return 3;
        }

        return 4;
    }

    uint32_t segment_heap::segment_signature() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::segment_heap_signature_field_symbol_name);
    }

    uint32_t segment_heap::global_flags() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::segment_heap_global_flags_field_symbol_name);
    }

    uint64_t segment_heap::total_reserved_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_mem_stats_total_reserved_pages_field_symbol_name);
    }

    uint64_t segment_heap::total_committed_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_mem_stats_total_committed_pages_field_symbol_name);
    }

    uint64_t segment_heap::free_committed_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_mem_stats_free_committed_pages_field_symbol_name);
    }

    uint64_t segment_heap::lfh_free_committed_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_mem_stats_lfh_free_committed_pages_field_symbol_name);
    }

    uint64_t segment_heap::large_reserved_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_large_reserved_pages_field_symbol_name);
    }

    uint64_t segment_heap::large_committed_pages() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::segment_heap_large_committed_pages_field_symbol_name);
    }

    size_units::base_10::bytes segment_heap::reserved() const
    {
        return size_units::base_10::bytes{total_reserved_pages() * peb().page_size()};
    }

    size_units::base_10::bytes segment_heap::committed() const
    {
        return size_units::base_10::bytes{total_committed_pages() * peb().page_size()};
    }

    size_units::base_10::bytes segment_heap::uncommitted() const
    {
        return reserved() - committed();
    }

    size_units::base_10::bytes segment_heap::large_reserved() const
    {
        return size_units::base_10::bytes{large_reserved_pages() * peb().page_size()};
    }

    size_units::base_10::bytes segment_heap::large_committed() const
    {
        return size_units::base_10::bytes{large_committed_pages() * peb().page_size()};
    }

    size_units::base_10::bytes segment_heap::large_uncommitted() const
    {
        return large_reserved() - large_committed();
    }

    std::experimental::generator<heap_segment_context> segment_heap::segment_contexts() const
    {
        if(auto const array_count = heap_seg_context_array_field_symbol_type_.array_count(); array_count.has_value())
        {
            auto array_field_address = segment_heap_address() + heap_seg_context_array_field_offset_;
            for(size_t index = 0; index < array_count.value(); ++index, array_field_address += heap_seg_context_symbol_length_)
            {
                co_yield heap_segment_context{*this, array_field_address};
            }
        }
    }

    heap_vs_context segment_heap::vs_context() const
    {
        return heap_vs_context{*this, segment_heap_address() + heap_vs_context_offset_};
    }

    heap_lfh_context segment_heap::lfh_context() const
    {
        return heap_lfh_context{*this, segment_heap_address() + heap_lfh_context_offset_};
    }

    std::optional<dph_heap> segment_heap::debug_page_heap() const
    {
        if(!peb().heap_page_alloc_enabled())
        {
            return std::nullopt;
        }

        for(auto const& heap : dph_heap::dph_heaps(peb()))
        {
            if(heap.normal_heap() == segment_heap_address())
            {
                return heap;
            }
        }

        return std::nullopt;
    }

    std::experimental::generator<large_alloc_entry> segment_heap::large_entries() const
    {
        for(ntdll_utilities::rtl_rb_tree_walker const rb_tree_walker{walker()
            , segment_heap_address() + stream_utils::get_field_offset(segment_heap_symbol_type_, symbol_name, common_symbol_names::segment_heap_large_alloc_metadata_field_symbol_name)
            , large_alloc_entry::symbol_name
            , common_symbol_names::heap_large_alloc_tree_node_field_symbol_name};
            auto const entry_address : rb_tree_walker.entries())
        {
            co_yield large_alloc_entry{*this, entry_address};
        }
    }

    template <typename T>
    void segment_heap::decode_vs_check_header_type(uint64_t header_address, void* header) const
    {
        T sizes;
        memcpy(&sizes, header, sizeof T);

        auto decoded_sizes = sizes ^ static_cast<T>(header_address) ^ static_cast<T>(heap_key());
        memcpy(header, &decoded_sizes, sizeof T);

        auto* segment_page_offset = static_cast<uint8_t*>(header) + sizeof T;
        *segment_page_offset ^= static_cast<uint8_t>(header_address) ^ static_cast<uint8_t>(heap_key());
    }

    void segment_heap::decode_vs_check_header(uint64_t const header_address, void* header) const
    {
        if(peb().is_x64_target())
        {
            decode_vs_check_header_type<uint64_t>(header_address, header);
            return;
        }

        if(peb().is_x86_target())
        {
            decode_vs_check_header_type<uint32_t>(header_address, header);
            return;
        }


        throw exceptions::wide_runtime_error{ L"Unsupported vs check header decode platform"s };
    }
}
