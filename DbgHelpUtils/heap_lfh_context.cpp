#include "heap_lfh_context.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_lfh_bucket.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_context::symbol_name = common_symbol_names::heap_lfh_context_structure_symbol_name;

    heap_lfh_context::heap_lfh_context(segment_heap const& heap, uint64_t const heap_lfh_context_address)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_lfh_context_address_{heap_lfh_context_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_lfh_context::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_context::peb() const
    {
        return heap().peb();
    }

    ust_address_stack_trace const& heap_lfh_context::stack_trace() const
    {
        return heap().stack_trace();
    }

    uint8_t heap_lfh_context::max_affinity() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_lfh_context_max_affinity_field_data, common_symbol_names::heap_lfh_context_max_affinity_field_symbol_name);
    }

    size_units::base_16::bytes heap_lfh_context::max_block_size() const
    {
        return size_units::base_16::bytes{stream_utils::get_field_value<uint16_t>(*this, cache_data_->heap_lfh_context_config_max_block_size_field_data, common_symbol_names::heap_lfh_context_config_max_block_size_field_symbol_name)};
    }

    bool heap_lfh_context::with_old_page_crossing_blocks() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, cache_data_->heap_lfh_context_config_with_old_page_crossing_blocks_field_data, common_symbol_names::heap_lfh_context_config_with_old_page_crossing_blocks_field_symbol_name) != 0x0;
    }

    bool heap_lfh_context::disable_randomization() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, cache_data_->heap_lfh_context_config_disable_randomization_field_data, common_symbol_names::heap_lfh_context_config_disable_randomization_field_symbol_name) != 0x0;
    }

    std::experimental::generator<heap_lfh_bucket> heap_lfh_context::active_buckets() const
    {
        if(auto const array_count = cache_data_->heap_lfh_context_buckets_field_symbol_type.array_count(); array_count.has_value())
        {
            if(auto const type = cache_data_->heap_lfh_context_buckets_field_symbol_type.type(); type.has_value() && type.value().sym_tag() == dbg_help::sym_tag_enum::PointerType)
            {
                if(auto const length = type.value().length(); length.has_value())
                {
                    auto array_field_address = heap_lfh_context_address() + cache_data_->heap_lfh_context_buckets_field_offset;
                    for(size_t index = 0; index < array_count.value(); ++index, array_field_address += length.value())
                    {
                        std::optional<uint64_t> bucket_address;
                        switch(length.value())
                        {
                        case sizeof(uint32_t):
                            bucket_address = stream_utils::read_field_value<uint32_t>(walker(), array_field_address);
                            break;
                        case sizeof(uint64_t):
                            bucket_address = stream_utils::read_field_value<uint64_t>(walker(), array_field_address);
                            break;

                        default:
                            break;
                        }

                        co_yield heap_lfh_bucket{*this, bucket_address.value(), static_cast<int16_t>(index)};
                    }
                }
            }
        }
    }

    void heap_lfh_context::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_lfh_context_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            auto const type_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_context_symbol_type, symbol_name, common_symbol_names::heap_lfh_context_buckets_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
            data.heap_lfh_context_buckets_field_symbol_type = type_data.type;
            data.heap_lfh_context_buckets_field_offset = type_data.field_offset;

            data.heap_lfh_context_max_affinity_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_context_symbol_type, symbol_name, common_symbol_names::heap_lfh_context_max_affinity_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_context_config_max_block_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_context_symbol_type, symbol_name, common_symbol_names::heap_lfh_context_config_max_block_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_context_config_with_old_page_crossing_blocks_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_context_symbol_type, symbol_name, common_symbol_names::heap_lfh_context_config_with_old_page_crossing_blocks_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_context_config_disable_randomization_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_context_symbol_type, symbol_name, common_symbol_names::heap_lfh_context_config_disable_randomization_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
        }
    }
}
