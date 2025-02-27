#include "crt_heap.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "crt_entry.h"
#include "process_environment_block.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    crt_heap::crt_heap(cache_manager& cache, process::process_environment_block const& peb)
    : cache_manager_{&cache}
    , peb_{&peb}
    , crt_first_block_{get_crt_first_block()}
    {
    }

    void crt_heap::set_base_diff_filter(crt_heap& base_diff_filter)
    {
        base_diff_filter_ = &base_diff_filter;
    }

    void crt_heap::clear_base_diff_filter()
    {
        base_diff_filter_ = nullptr;
    }

    stream_stack_dump::mini_dump_memory_walker const& crt_heap::walker() const
    {
        return peb().walker();
    }

    dlg_help_utils::generator<crt_entry> crt_heap::entries() const
    {
        if(!is_using_crt_heap())
        {
            co_return;
        }

        std::vector<crt_entry> filters;
        std::ranges::copy(filter_entries(), std::back_inserter(filters));

        for (auto const& entry : all_entries())
        {
            if(!is_filtered(filters, entry))
            {
                co_yield entry;
            }
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

    dlg_help_utils::generator<crt_entry> crt_heap::all_entries() const
    {
        if(!is_using_crt_heap())
        {
            co_return;
        }

        auto address = crt_first_block_;
        while(address != 0)
        {
            co_yield crt_entry{*this, address};
            address = get_field_pointer_raw(walker(), address, cache_data_->crt_mem_block_header_block_header_next_field_data, crt_entry::symbol_name, common_symbol_names::crt_mem_block_header_block_header_next_field_symbol_name);
        }
    }

    dlg_help_utils::generator<crt_entry> crt_heap::filter_entries() const
    {
        if(base_diff_filter_ != nullptr)
        {
            for (auto const& entry : base_diff_filter_->entries())
            {
                co_yield entry;
            }
        }
    }

    bool crt_heap::is_filtered(std::vector<crt_entry> const& filters, crt_entry const& entry)
    {
        return std::ranges::find(filters, entry) != filters.end();
    }

    crt_heap::cache_data const& crt_heap::setup_globals() const
    {
        if(is_using_crt_heap() && !cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.block_symbol_symbol_type = stream_utils::get_type(walker(), crt_entry::symbol_name);
            data.crt_mem_block_header_block_header_next_field_data = stream_utils::get_field_type_and_offset_in_type(data.block_symbol_symbol_type, crt_entry::symbol_name, common_symbol_names::crt_mem_block_header_block_header_next_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            crt_entry::setup_globals(*this);
        }

        return cache().get_cache<cache_data>();
    }
}
