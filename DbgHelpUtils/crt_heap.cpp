#include "crt_heap.h"

#include "common_symbol_names.h"
#include "crt_entry.h"
#include "process_environment_block.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    crt_heap::crt_heap(process::process_environment_block const& peb)
    : peb_{peb}
    , crt_first_block_{get_crt_first_block()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& crt_heap::walker() const
    {
        return peb().walker();
    }

    std::experimental::generator<crt_entry> crt_heap::entries() const
    {
        if(!is_using_crt_heap())
        {
            co_return;
        }

        auto const block_symbol = stream_utils::get_type(walker(), crt_entry::symbol_name);
        auto address = crt_first_block_;
        while(address != 0)
        {
            co_yield crt_entry{*this, address};

            address = stream_utils::get_field_pointer_raw(walker(), address, block_symbol, crt_entry::symbol_name, common_symbol_names::crt_mem_block_header_block_header_next_field_symbol_name);
        }
    }

    uint64_t crt_heap::get_crt_first_block() const
    {
        auto const symbol = walker().get_symbol_info(common_symbol_names::acrt_first_block);
        if(!symbol.has_value())
        {
            return 0;
        }

        auto const address = symbol.value().address();
        if(!address.has_value())
        {
            return 0;
        }

        auto const value = stream_utils::read_machine_size_field_value(peb(), address.value());
        if(!value.has_value())
        {
            return 0;
        }

        return value.value();
    }
}
