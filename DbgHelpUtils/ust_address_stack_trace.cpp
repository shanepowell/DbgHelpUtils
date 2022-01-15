#include "ust_address_stack_trace.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "process_environment_block.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    ust_address_stack_trace::ust_address_stack_trace(cache_manager& cache, stream_stack_dump::mini_dump_stack_walk const& walker)
    : cache_manager_{cache}
    , walker_{walker}
    {
    }

    std::vector<uint64_t> ust_address_stack_trace::read_allocation_stack_trace(process::process_environment_block const& peb, uint64_t const ust_address) const
    {
        std::vector<uint64_t> trace;
        if(ust_address == 0 || !has_ust_setup(peb))
        {
            return trace;
        }

        auto const depth = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker_, get_ust_address_depth_field(peb), ust_address);
        if(!depth.has_value())
        {
            return trace;
        }

        auto const back_trace_address = ust_address + cache_data_.rtl_stack_trace_entry_back_trace_field_data.field_offset;

        trace.reserve(depth.value());

        for(uint16_t index = 0; index < depth.value(); ++index)
        {
            auto const sp = stream_utils::find_field_pointer_array_type_and_value(walker_, cache_data_.rtl_stack_trace_entry_back_trace_field_data.type, back_trace_address, index);
            if(!sp.has_value())
            {
                break;
            }

            trace.push_back(sp.value().second);
        }

        return trace;
    }

    bool ust_address_stack_trace::is_valid_ust_address(process::process_environment_block const& peb, uint64_t const ust_address) const
    {
        if(!has_ust_setup(peb) || ust_address == 0 || ust_address == 0x01010000 || ust_address == 0x0101000000000000)
        {
            return false;
        }

        if(peb.walker().find_memory_range(ust_address, 0x01, cache_data_.rtl_stack_trace_entry_symbol_length) != cache_data_.rtl_stack_trace_entry_symbol_length)
        {
            return false;
        }

        auto const depth = stream_utils::find_basic_type_field_value_in_type<uint16_t>(peb.walker(), get_ust_address_depth_field(peb), ust_address);
        if(!depth.has_value())
        {
            return false;
        }

        auto const array_count = cache_data_.rtl_stack_trace_entry_back_trace_field_data.type.array_count();
        if(!array_count.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::rtl_stack_trace_entry_structure_symbol_name, common_symbol_names::rtl_stack_trace_entry_back_trace_field_symbol_name);
        }

        return depth > 0 && depth < array_count.value();
    }

    stream_utils::symbol_type_and_base_type_field_offset const& ust_address_stack_trace::get_ust_address_depth_field(process::process_environment_block const& peb) const
    {
        if(peb.heap_page_alloc_enabled())
        {
            return cache_data_.rtl_stack_trace_entry_depth_field_data;
        }

        if(peb.user_stack_db_enabled())
        {
            return cache_data_.rtl_stack_trace_entry_index_field_data;
        }

        throw exceptions::wide_runtime_error{L"Error: ust or hpa not enabled"s};
    }

    ust_address_stack_trace::cache_data const& ust_address_stack_trace::setup_globals() const
    {
        if(!cache_manager_.has_cache<cache_data>())
        {
            auto& data = cache_manager_.get_cache<cache_data>();
            data.rtl_stack_trace_entry_symbol_type = stream_utils::get_type(walker_, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name);
            data.rtl_stack_trace_entry_symbol_length = stream_utils::get_type_length(data.rtl_stack_trace_entry_symbol_type, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name);

            data.rtl_stack_trace_entry_depth_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_stack_trace_entry_symbol_type, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name, common_symbol_names::rtl_stack_trace_entry_depth_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.rtl_stack_trace_entry_index_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_stack_trace_entry_symbol_type, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name, common_symbol_names::rtl_stack_trace_entry_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.rtl_stack_trace_entry_back_trace_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_stack_trace_entry_symbol_type, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name, common_symbol_names::rtl_stack_trace_entry_back_trace_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
        }

        return cache_manager_.get_cache<cache_data>();
    }

    bool ust_address_stack_trace::has_ust_setup(process::process_environment_block const& peb)
    {
        return peb.heap_page_alloc_enabled() || peb.user_stack_db_enabled();
    }
}
