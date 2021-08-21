#include "process_environment_block.h"

#include <sstream>

#include "common_symbol_names.h"
#include "common_symbol_utils.h"
#include "heap_segment.h"
#include "nt_heap.h"
#include "process_parameters.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::process
{
    process_environment_block::process_environment_block(dlg_help_utils::mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine)
    : mini_dump_{mini_dump}
    , names_list_{mini_dump}
    , memory_list_{mini_dump}
    , memory64_list_{mini_dump}
    , function_table_{mini_dump}
    , module_list_{mini_dump}
    , unloaded_module_list_{mini_dump}
    , walker_{0, nullptr, 0, memory_list_, memory64_list_, function_table_, module_list_, unloaded_module_list_, pe_file_memory_mappings_, symbol_engine}
    , heap_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::heap_structure_symbol_name)}
    , peb_symbol_info_{stream_utils::get_type(walker(), common_symbol_names::peb_structure_symbol_name)}
    , teb_symbol_info_{stream_utils::get_type(walker(), common_symbol_names::teb_structure_symbol_name)}
    , teb_address_{get_teb_address(mini_dump, walker(), names_list_)}
    , peb_address_{get_peb_address(walker(), teb_symbol_info_, teb_address_)}
    , nt_global_flag_{get_nt_global_flag(walker(), peb_symbol_info_, peb_address())}
    , process_heap_{get_main_process_heap(walker(), peb_symbol_info_, peb_address())}
    , number_of_heaps_{get_number_of_heaps(walker(), peb_symbol_info_, peb_address())}
    , ldr_address_{get_ldr_address(walker(), peb_symbol_info_, peb_address())}
    , process_heaps_pointer_type_{0, 0}
    {
        const auto process_environment_block_address = stream_utils::find_field_pointer_type_and_value_in_type(walker(), peb_symbol_info_, common_symbol_names::peb_structure_process_heaps_field_symbol_name, peb_address_);
        if(!process_environment_block_address.has_value() || process_environment_block_address.value().second == 0)
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_process_heaps_field_symbol_name);
        }

        process_heaps_pointer_type_ = process_environment_block_address.value().first;
        process_heaps_address_ = process_environment_block_address.value().second;

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
        const auto process_parameters_address = stream_utils::find_field_pointer_type_and_value_in_type(walker(), peb_symbol_info_, common_symbol_names::peb_structure_process_parameters_field_symbol_name, peb_address());
        if(!process_parameters_address.has_value() || process_parameters_address.value().second == 0)
        {
            return std::nullopt;
        }

        return process::process_parameters{walker(), process_parameters_address.value().second};
    }

    uint64_t process_environment_block::heap_address(uint32_t const heap_index) const
    {
        auto const heap = stream_utils::get_field_pointer_type_and_value(walker(), process_heaps_pointer_type_, process_heaps_address_, heap_index);
        if(!heap.has_value())
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} <<  "Error: Failed to find heap index " << heap_index).str()};
        }

        return heap.value().second;
    }

    uint32_t process_environment_block::segment_signature(uint32_t const heap_index) const
    {
        heap::nt_heap const nt_heap{*this, heap_address(heap_index)};
        return nt_heap.segment_signature();
    }

    std::optional<heap::nt_heap> process_environment_block::nt_heap(uint32_t const heap_index) const
    {
        heap::nt_heap nt_heap{*this, heap_address(heap_index)};
        if(nt_heap.segment_signature() != heap::SegmentSignatureNtHeap)
        {
            return std::nullopt;
        }
        return std::move(nt_heap);
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

    uint64_t process_environment_block::get_teb_address(dlg_help_utils::mini_dump const& mini_dump, stream_stack_dump::mini_dump_stack_walk const& walker, thread_names_list_stream const& names_list)
    {
        auto const teb_address = common_symbol_utils::get_teb_address(mini_dump, names_list, walker.memory_list(), walker.memory64_list());
        if(!teb_address.has_value())
        {
            throw exceptions::wide_runtime_error{L"Error: No TEB address found"s};
        }

        return teb_address.value();
    }

    uint64_t process_environment_block::get_peb_address(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& teb_symbol_info, uint64_t const teb_address)
    {
        const auto peb_address = stream_utils::find_field_pointer_type_and_value_in_type(walker, teb_symbol_info, common_symbol_names::teb_structure_process_environment_block_field_symbol_name, teb_address);
        if(!peb_address.has_value() || peb_address.value().second == 0)
        {
            throw exceptions::wide_runtime_error{L"Error: No PEB address found"s};
        }

        return peb_address.value().second;
    }

    gflags_utils::gflags process_environment_block::get_nt_global_flag(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t const peb_address)
    {
        auto const nt_global_flag_value = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, peb_symbol_info, common_symbol_names::peb_structure_nt_global_flag_field_symbol_name, peb_address);
        if(!nt_global_flag_value.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_nt_global_flag_field_symbol_name);
        }

        return static_cast<gflags_utils::gflags>(nt_global_flag_value.value());
    }

    uint64_t process_environment_block::get_main_process_heap(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t const peb_address)
    {
        const auto process_heap = stream_utils::find_field_pointer_type_and_value_in_type(walker, peb_symbol_info, common_symbol_names::peb_structure_process_heap_field_symbol_name, peb_address);
        if(!process_heap.has_value() || process_heap.value().second == 0)
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_process_heap_field_symbol_name);
        }

        return process_heap.value().second;
    }

    uint32_t process_environment_block::get_number_of_heaps(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t const peb_address)
    {
        auto const number_of_heaps = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, peb_symbol_info, common_symbol_names::peb_structure_number_of_heaps_field_symbol_name, peb_address);
        if(!number_of_heaps.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_number_of_heaps_field_symbol_name);
        }

        return number_of_heaps.value();
    }

    uint64_t process_environment_block::get_ldr_address(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t const peb_address)
    {
        const auto process_heap = stream_utils::find_field_pointer_type_and_value_in_type(walker, peb_symbol_info, common_symbol_names::peb_structure_ldr_field_symbol_name, peb_address);
        if(!process_heap.has_value())
        {
            stream_utils::throw_cant_get_symbol_field(common_symbol_names::peb_structure_ldr_field_symbol_name);
        }

        return process_heap.value().second;
    }
}
