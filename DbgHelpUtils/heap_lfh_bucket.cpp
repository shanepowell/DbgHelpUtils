#include "heap_lfh_bucket.h"

#include "common_symbol_names.h"
#include "heap_lfh_affinity_slot.h"
#include "heap_lfh_context.h"
#include "process_environment_block.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_bucket::symbol_name = common_symbol_names::heap_lfh_bucket_structure_symbol_name;

    heap_lfh_bucket::heap_lfh_bucket(heap_lfh_context const& heap, uint64_t const heap_lfh_bucket_address)
    : heap_{heap}
    , heap_lfh_bucket_address_{heap_lfh_bucket_address}
    , heap_lfh_bucket_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , bucket_index_{get_bucket_index()}
    , bucket_granularity_{get_bucket_granularity()}
    , max_allocation_size_{get_max_allocation_size()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_lfh_bucket::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_bucket::peb() const
    {
        return heap().peb();
    }

    size_t heap_lfh_bucket::slot_count() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_bucket_slot_count_field_symbol_name);
    }

    uint64_t heap_lfh_bucket::total_block_count() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_lfh_bucket_total_block_count_field_symbol_name);
    }

    uint64_t heap_lfh_bucket::total_subsegment_count() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_lfh_bucket_total_subsegment_count_field_symbol_name);
    }

    size_units::base_10::bytes heap_lfh_bucket::reciprocal_block_size() const
    {
        return size_units::base_10::bytes{stream_utils::get_field_value<uint32_t>(*this, common_symbol_names::heap_lfh_bucket_reciprocal_block_size_field_symbol_name)};
    }

    uint8_t heap_lfh_bucket::shift() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_bucket_shift_field_symbol_name);
    }

    std::vector<uint8_t> heap_lfh_bucket::processor_affinity_mapping() const
    {
        auto const proc_affinity_mapping_address = stream_utils::get_field_pointer(*this, common_symbol_names::heap_lfh_bucket_proc_affinity_mapping_field_symbol_name);

        std::vector<uint8_t> rv;
        rv.resize(heap().max_affinity());
        
        if(auto stream = walker().get_process_memory_stream(proc_affinity_mapping_address, heap().max_affinity());
            stream.eof() || stream.read(rv.data(), rv.size()) != rv.size())
        {
            stream_utils::throw_cant_get_field_is_null(symbol_name, common_symbol_names::heap_lfh_bucket_proc_affinity_mapping_field_symbol_name);
        }
        return rv;
    }

    std::experimental::generator<heap_lfh_affinity_slot> heap_lfh_bucket::affinity_slots() const
    {
        auto const max_slots{slot_count()};
        auto affinity_slots_address = stream_utils::get_field_pointer(*this, common_symbol_names::heap_lfh_bucket_affinity_slots_field_symbol_name);
        auto const pointer_size{peb().is_x86_target() ? sizeof uint32_t : sizeof uint64_t};

        for(size_t index = 0; index < max_slots; ++index)
        {
            if(auto const slot_address = stream_utils::read_machine_size_field_value(peb(), affinity_slots_address); slot_address.has_value())
            {
                co_yield heap_lfh_affinity_slot{heap(), slot_address.value()};
            }
            affinity_slots_address += pointer_size;
        }
    }

    namespace
    {
        struct BucketIndexMapData
        {
            uint32_t offset;
            uint32_t granularity;
            uint32_t base_size;
            uint32_t index_adjustment;
        };

        const std::map<size_t, BucketIndexMapData> bucket_index_map
        {
            { 64, { 0, 16, 0, 0 } },
            { 80, { 65, 64, 0x400, 1 } },
            { 96, { 81, 128, 0x800, 1 } },
            { 112, { 97, 256, 0x1000, 1 } },
            { 0xFFFF, { 113, 512, 0x2000, 1 } }
        };
    }

    size_t heap_lfh_bucket::get_bucket_index() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_bucket_bucket_index_field_symbol_name);
    }

    size_units::base_10::bytes heap_lfh_bucket::get_bucket_granularity() const
    {
        return size_units::base_10::bytes{bucket_index_map.lower_bound(bucket_index())->second.granularity};
    }

    size_units::base_10::bytes heap_lfh_bucket::get_max_allocation_size() const
    {
        auto const& bucket_data = bucket_index_map.lower_bound(bucket_index());
        return size_units::base_10::bytes{bucket_data->second.base_size + bucket_data->second.granularity * (bucket_index() - bucket_data->second.offset + bucket_data->second.index_adjustment)};
    }
}
