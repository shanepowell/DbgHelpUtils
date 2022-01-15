#include "process_environment_block.h"

#include <format>

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "common_symbol_utils.h"
#include "heap_segment.h"
#include "nt_heap.h"
#include "process_parameters.h"
#include "segment_heap.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::process
{
    process_environment_block::process_environment_block(dlg_help_utils::mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine)
    : cache_manager_{cache}
    , mini_dump_{mini_dump}
    , names_list_{mini_dump}
    , memory_list_{mini_dump}
    , memory64_list_{mini_dump}
    , function_table_{mini_dump}
    , module_list_{mini_dump}
    , unloaded_module_list_{mini_dump}
    , system_memory_info_{mini_dump}
    , walker_{0, nullptr, 0, memory_list_, memory64_list_, function_table_, module_list_, unloaded_module_list_, pe_file_memory_mappings_, symbol_engine}
    {
        const auto process_environment_block_address = find_field_pointer_type_and_value_in_type(walker(), cache_data_.peb_structure_process_heaps_field_data, peb_address_);
        if(!process_environment_block_address.has_value() || process_environment_block_address.value().value == 0)
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_process_heaps_field_symbol_name);
        }

        process_heaps_pointer_type_ = process_environment_block_address.value().type;
        process_heaps_address_ = process_environment_block_address.value().value;

        auto const pointer_length = process_heaps_pointer_type_.length();
        if(!pointer_length.has_value())
        {
            throw exceptions::wide_runtime_error{L"Error: No pointer length found!"s};
        }

        machine_pointer_size_ = static_cast<std::streamsize>(pointer_length.value());
        machine_hex_printable_length_ = machine_pointer_size_ * 2;
    }

    std::optional<process_parameters> process_environment_block::process_parameters() const
    {
        const auto process_parameters_address = find_field_pointer_type_and_value_in_type(walker(), cache_data_.peb_structure_process_parameters_field_data, peb_address());
        if(!process_parameters_address.has_value() || process_parameters_address.value().value == 0)
        {
            return std::nullopt;
        }

        return process::process_parameters{cache_manager_, walker(), process_parameters_address.value().value};
    }

    uint64_t process_environment_block::heap_address(uint32_t const heap_index) const
    {
        auto const heap = stream_utils::find_field_pointer_type_and_value(walker(), process_heaps_pointer_type_, process_heaps_address_, heap_index);
        if(!heap.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: Failed to find heap index {}", heap_index)};
        }

        return heap.value().second;
    }

    uint32_t process_environment_block::segment_signature(uint32_t const heap_index) const
    {
        heap::nt_heap const nt_heap{cache_manager_, *this, heap_address(heap_index)};
        return nt_heap.segment_signature();
    }

    std::optional<heap::nt_heap> process_environment_block::nt_heap(uint32_t const heap_index) const
    {
        heap::nt_heap heap{cache_manager_, *this, heap_address(heap_index)};
        if(heap.segment_signature() != heap::SegmentSignatureNtHeap)
        {
            return std::nullopt;
        }
        return std::move(heap);
    }

    std::optional<heap::segment_heap> process_environment_block::segment_heap(uint32_t const heap_index) const
    {
        heap::segment_heap heap{cache_manager_, *this, heap_address(heap_index)};
        if(heap.segment_signature() != heap::SegmentSignatureSegmentHeap)
        {
            return std::nullopt;
        }
        return std::optional{std::move(heap)};
    }

    bool process_environment_block::is_x86_target() const
    {
        return machine_pointer_size_ == 0x04;
    }

    bool process_environment_block::is_x64_target() const
    {
        return machine_pointer_size_ == 0x08;
    }

    bool process_environment_block::user_stack_db_enabled() const
    {
        return (static_cast<uint32_t>(nt_global_flag_) & static_cast<uint32_t>(gflags_utils::gflags::FLG_USER_STACK_TRACE_DB)) == static_cast<uint32_t>(gflags_utils::gflags::FLG_USER_STACK_TRACE_DB);
    }

    bool process_environment_block::heap_page_alloc_enabled() const
    {
        return (static_cast<uint32_t>(nt_global_flag_) & static_cast<uint32_t>(gflags_utils::gflags::FLG_HEAP_PAGE_ALLOCS)) == static_cast<uint32_t>(gflags_utils::gflags::FLG_HEAP_PAGE_ALLOCS);
    }

    uint64_t process_environment_block::get_teb_address() const
    {
        auto const teb_address = common_symbol_utils::get_teb_address(mini_dump_, names_list_, walker().memory_list(), walker().memory64_list());
        if(!teb_address.has_value())
        {
            throw exceptions::wide_runtime_error{L"Error: No TEB address found"s};
        }

        return teb_address.value();
    }

    uint64_t process_environment_block::get_peb_address() const
    {
        const auto peb_address = find_field_pointer_type_and_value_in_type(walker(), cache_data_.teb_structure_process_environment_block_field_data, teb_address_);
        if(!peb_address.has_value() || peb_address.value().value == 0)
        {
            throw exceptions::wide_runtime_error{L"Error: No PEB address found"s};
        }

        return peb_address.value().value;
    }

    gflags_utils::gflags process_environment_block::get_nt_global_flag() const
    {
        auto const nt_global_flag_value = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), cache_data_.peb_structure_nt_global_flag_field_data, peb_address());
        if(!nt_global_flag_value.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_nt_global_flag_field_symbol_name);
        }

        return static_cast<gflags_utils::gflags>(nt_global_flag_value.value());
    }

    uint64_t process_environment_block::get_main_process_heap() const
    {
        const auto process_heap = find_field_pointer_type_and_value_in_type(walker(), cache_data_.peb_structure_process_heaps_field_data, peb_address());
        if(!process_heap.has_value() || process_heap.value().value == 0)
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_process_heap_field_symbol_name);
        }

        return process_heap.value().value;
    }

    uint32_t process_environment_block::get_number_of_heaps() const
    {
        auto const number_of_heaps = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), cache_data_.peb_structure_number_of_heaps_field_data, peb_address());
        if(!number_of_heaps.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_number_of_heaps_field_symbol_name);
        }

        return number_of_heaps.value();
    }

    uint64_t process_environment_block::get_ldr_address() const
    {
        const auto process_heap = find_field_pointer_type_and_value_in_type(walker(), cache_data_.peb_structure_ldr_field_data, peb_address());
        if(!process_heap.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_ldr_field_symbol_name);
        }

        return process_heap.value().value;
    }

    process_environment_block::cache_data const& process_environment_block::setup_globals() const
    {
        if(!cache_manager_.has_cache<cache_data>())
        {
            auto& data = cache_manager_.get_cache<cache_data>();
            data.heap_symbol_type = stream_utils::get_type(walker(), common_symbol_names::heap_structure_symbol_name);
            data.peb_symbol_info = stream_utils::get_type(walker(), common_symbol_names::peb_structure_symbol_name);
            data.teb_symbol_info = stream_utils::get_type(walker(), common_symbol_names::teb_structure_symbol_name);

            data.peb_structure_process_heaps_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_process_heaps_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.peb_structure_process_parameters_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_process_heaps_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.peb_structure_process_heap_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_process_heap_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.peb_structure_ldr_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_ldr_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.peb_structure_nt_global_flag_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_nt_global_flag_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.peb_structure_number_of_heaps_field_data = stream_utils::get_field_type_and_offset_in_type(data.peb_symbol_info, common_symbol_names::peb_structure_symbol_name, common_symbol_names::peb_structure_number_of_heaps_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.teb_structure_process_environment_block_field_data = stream_utils::get_field_type_and_offset_in_type(data.teb_symbol_info, common_symbol_names::teb_structure_symbol_name, common_symbol_names::teb_structure_process_environment_block_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache_manager_.get_cache<cache_data>();
    }
}
