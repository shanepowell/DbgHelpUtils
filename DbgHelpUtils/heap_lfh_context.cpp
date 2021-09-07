#include "heap_lfh_context.h"

#include "common_symbol_names.h"
#include "heap_lfh_bucket.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_context::symbol_name = common_symbol_names::heap_lfh_context_structure_symbol_name;

    heap_lfh_context::heap_lfh_context(segment_heap const& heap, uint64_t const heap_lfh_context_address)
    : heap_{heap}
    , heap_lfh_context_address_{heap_lfh_context_address}
    , heap_lfh_context_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_lfh_context_buckets_field_symbol_type_{get_heap_lfh_context_buckets_array_field_symbol_type()}
    , heap_lfh_context_buckets_field_offset_{get_heap_lfh_context_buckets_array_field_offset()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_lfh_context::walker() const
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
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_context_max_affinity_field_symbol_name);
    }

    size_units::base_10::bytes heap_lfh_context::max_block_size() const
    {
        return size_units::base_10::bytes{stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_lfh_context_config_max_block_size_field_symbol_name)};
    }

    bool heap_lfh_context::with_old_page_crossing_blocks() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_lfh_context_config_with_old_page_crossing_blocks_field_symbol_name) != 0x0;
    }

    bool heap_lfh_context::disable_randomization() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_lfh_context_config_disable_randomization_field_symbol_name) != 0x0;
    }

    std::experimental::generator<heap_lfh_bucket> heap_lfh_context::active_buckets() const
    {
        if(auto const array_count = heap_lfh_context_buckets_field_symbol_type_.array_count(); array_count.has_value())
        {
            if(auto const type = heap_lfh_context_buckets_field_symbol_type_.type(); type.has_value() && type.value().sym_tag() == dbg_help::sym_tag_enum::PointerType)
            {
                if(auto const length = type.value().length(); length.has_value())
                {
                    auto array_field_address = heap_lfh_context_address() + heap_lfh_context_buckets_field_offset_;
                    for(size_t index = 0; index < array_count.value(); ++index, array_field_address += length.value())
                    {
                        std::optional<uint64_t> bucket_address;
                        switch(length.value())
                        {
                        case sizeof uint32_t:
                            bucket_address = stream_utils::read_field_value<uint32_t>(walker(), array_field_address);
                            break;
                        case sizeof uint64_t:
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

    std::pair<dbg_help::symbol_type_info, uint64_t> heap_lfh_context::get_heap_lfh_context_buckets_array_data() const
    {
        const auto seg_context_array = stream_utils::find_field_type_and_offset_in_type(heap_lfh_context_symbol_type_, common_symbol_names::heap_lfh_context_buckets_field_symbol_name, dbg_help::sym_tag_enum::ArrayType);
        if(!seg_context_array.has_value())
        {
            stream_utils::throw_cant_get_field_data(symbol_name, common_symbol_names::heap_lfh_context_buckets_field_symbol_name);
        }
        return seg_context_array.value();
    }

    dbg_help::symbol_type_info heap_lfh_context::get_heap_lfh_context_buckets_array_field_symbol_type() const
    {
        return std::get<0>(get_heap_lfh_context_buckets_array_data());
    }

    uint64_t heap_lfh_context::get_heap_lfh_context_buckets_array_field_offset() const
    {
        return std::get<1>(get_heap_lfh_context_buckets_array_data());
    }
}
