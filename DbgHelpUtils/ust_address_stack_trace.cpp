#include "ust_address_stack_trace.h"

#include "common_symbol_names.h"
#include "process_environment_block.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    ust_address_stack_trace::ust_address_stack_trace(stream_stack_dump::mini_dump_stack_walk const& walker)
    : walker_{walker}
    {
    }

    dbg_help::symbol_type_info ust_address_stack_trace::get_rtl_stack_trace_entry_symbol_type() const
    {
        return stream_utils::get_type(walker_, common_symbol_names::rtl_stack_trace_entry_structure_symbol_name);
    }

    std::vector<uint64_t> ust_address_stack_trace::read_allocation_stack_trace(process::process_environment_block const& peb, uint64_t const ust_address) const
    {
        std::vector<uint64_t> trace;
        if(ust_address == 0)
        {
            return trace;
        }

        auto const depth = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker_, rtl_stack_trace_entry_symbol_type_, get_ust_address_depth_field(peb), ust_address);
        if(!depth.has_value())
        {
            return trace;
        }

        const auto back_trace_value = stream_utils::find_field_type_and_offset_in_type(rtl_stack_trace_entry_symbol_type_, common_symbol_names::rtl_stack_trace_entry_back_trace_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
        if(!back_trace_value.has_value())
        {
            return trace;
        }
        auto const back_trace_address = ust_address + back_trace_value.value().second;

        trace.reserve(depth.value());

        for(uint16_t index = 0; index < depth.value(); ++index)
        {
            auto const sp = stream_utils::get_field_pointer_array_type_and_value(walker_, back_trace_value.value().first, back_trace_address, index);
            if(!sp.has_value())
            {
                break;
            }

            trace.push_back(sp.value().second);
        }

        return trace;
    }

    std::wstring const& ust_address_stack_trace::get_ust_address_depth_field(process::process_environment_block const& peb)
    {
        if(peb.heap_page_alloc_enabled())
        {
            return common_symbol_names::rtl_stack_trace_entry_depth_field_symbol_name;
        }

        if(peb.user_stack_db_enabled())
        {
            return common_symbol_names::rtl_stack_trace_entry_index_field_symbol_name;
        }

        throw exceptions::wide_runtime_error{L"Error: ust or hpa not enabled"s};
    }
}
