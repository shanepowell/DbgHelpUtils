#include "heap_lfh_bucket.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_lfh_affinity_slot.h"
#include "heap_lfh_context.h"
#include "process_environment_block.h"
#include "segment_heap.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_bucket::symbol_name = common_symbol_names::heap_lfh_bucket_structure_symbol_name;

    heap_lfh_bucket::heap_lfh_bucket(heap_lfh_context const& heap, uint64_t const heap_lfh_bucket_address, int16_t const bucket_index)
    : cache_data_{&heap.heap().cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_lfh_bucket_address_{heap_lfh_bucket_address}
    , is_enabled_{(heap_lfh_bucket_address_ & 0x01) == 0x0}
    , bucket_index_{bucket_index == -1 ? get_bucket_index() : bucket_index}
    , bucket_granularity_{get_bucket_granularity()}
    , max_allocation_size_{get_max_allocation_size()}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_lfh_bucket::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_bucket::peb() const
    {
        return heap().peb();
    }

    uint16_t heap_lfh_bucket::usage_count() const
    {
        if(is_enabled())
        {
            throw exceptions::wide_runtime_error{L"LFH Bucket is enabled"s};
        }

        uint32_t value;
        void const* buffer;
        if(peb().is_x64_target())
        {
            buffer = &heap_lfh_bucket_address_;
        }
        else if(peb().is_x86_target())
        {
            value = static_cast<uint32_t>(heap_lfh_bucket_address_);
            buffer = &value;
        }
        else
        {
            throw exceptions::wide_runtime_error{L"Unsupported platform"s};
        }

        auto const usage_data = stream_utils::find_basic_type_field_value_in_type<uint16_t>(cache_data_->heap_lfh_ondemand_pointer_usage_data_field_data, buffer);
        if(!usage_data.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::heap_lfh_ondemand_pointer_structure_symbol_name, common_symbol_names::heap_lfh_ondemand_pointer_usage_data_field_symbol_name);
        }

        return usage_data.value();
    }

    size_t heap_lfh_bucket::slot_count() const
    {
        validate_enabled();
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_lfh_bucket_slot_count_field_data, common_symbol_names::heap_lfh_bucket_slot_count_field_symbol_name);
    }

    uint64_t heap_lfh_bucket::total_block_count() const
    {
        validate_enabled();
        return get_machine_size_field_value(*this, cache_data_->heap_lfh_bucket_total_block_count_field_data, common_symbol_names::heap_lfh_bucket_total_block_count_field_symbol_name);
    }

    uint64_t heap_lfh_bucket::total_subsegment_count() const
    {
        validate_enabled();
        return get_machine_size_field_value(*this, cache_data_->heap_lfh_bucket_total_subsegment_count_field_data, common_symbol_names::heap_lfh_bucket_total_subsegment_count_field_symbol_name);
    }

    size_units::base_16::bytes heap_lfh_bucket::reciprocal_block_size() const
    {
        validate_enabled();
        return size_units::base_16::bytes{stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_lfh_bucket_reciprocal_block_size_field_data, common_symbol_names::heap_lfh_bucket_reciprocal_block_size_field_symbol_name)};
    }

    uint8_t heap_lfh_bucket::shift() const
    {
        validate_enabled();
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_lfh_bucket_shift_field_data, common_symbol_names::heap_lfh_bucket_shift_field_symbol_name);
    }

    std::vector<uint8_t> heap_lfh_bucket::processor_affinity_mapping() const
    {
        validate_enabled();
        auto const proc_affinity_mapping_address = get_field_pointer(*this, cache_data_->heap_lfh_bucket_proc_affinity_mapping_field_data, common_symbol_names::heap_lfh_bucket_proc_affinity_mapping_field_symbol_name);

        std::vector<uint8_t> rv;
        rv.resize(heap().max_affinity());
        
        if(auto stream = walker().get_process_memory_stream(proc_affinity_mapping_address, heap().max_affinity());
            stream.eof() || stream.read(rv.data(), rv.size()) != rv.size())
        {
            stream_utils::throw_cant_get_field_is_null(symbol_name, common_symbol_names::heap_lfh_bucket_proc_affinity_mapping_field_symbol_name);
        }
        return rv;
    }

    generator<heap_lfh_affinity_slot> heap_lfh_bucket::affinity_slots() const
    {
        validate_enabled();
        auto const max_slots{slot_count()};
        auto affinity_slots_address = get_field_pointer(*this, cache_data_->heap_lfh_bucket_affinity_slots_field_data, common_symbol_names::heap_lfh_bucket_affinity_slots_field_symbol_name);
        auto const pointer_size{peb().is_x86_target() ? sizeof(uint32_t) : sizeof(uint64_t)};

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
            { 64, { .offset= 0, .granularity= 16, .base_size= 0, .index_adjustment= 0}},
            { 80, { .offset= 65, .granularity= 64, .base_size= 0x400, .index_adjustment= 1}},
            { 96, { .offset= 81, .granularity= 128, .base_size= 0x800, .index_adjustment= 1}},
            { 112, { .offset= 97, .granularity= 256, .base_size= 0x1000, .index_adjustment= 1}},
            { 0xFFFF, { .offset= 113, .granularity= 512, .base_size= 0x2000, .index_adjustment= 1}}
        };
    }

    void heap_lfh_bucket::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_lfh_bucket_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_lfh_bucket_slot_count_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_slot_count_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_total_block_count_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_total_block_count_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_total_subsegment_count_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_total_subsegment_count_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_reciprocal_block_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_reciprocal_block_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_shift_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_shift_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_bucket_index_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_bucket_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_bucket_proc_affinity_mapping_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_proc_affinity_mapping_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.heap_lfh_bucket_affinity_slots_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_bucket_symbol_type, symbol_name, common_symbol_names::heap_lfh_bucket_affinity_slots_field_symbol_name, dbg_help::sym_tag_enum::PointerType);

            data.heap_lfh_ondemand_pointer_symbol_type = stream_utils::get_type(heap.walker(), common_symbol_names::heap_lfh_ondemand_pointer_structure_symbol_name);
            data.heap_lfh_ondemand_pointer_usage_data_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_lfh_ondemand_pointer_symbol_type, common_symbol_names::heap_lfh_ondemand_pointer_structure_symbol_name, common_symbol_names::heap_lfh_ondemand_pointer_usage_data_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
        }
    }

    void heap_lfh_bucket::validate_enabled() const
    {
        if(!is_enabled())
        {
            throw exceptions::wide_runtime_error{L"LFH Bucket not enabled"s};
        }
    }

    size_t heap_lfh_bucket::get_bucket_index() const
    {
        validate_enabled();
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_->heap_lfh_bucket_bucket_index_field_data, common_symbol_names::heap_lfh_bucket_bucket_index_field_symbol_name);
    }

    size_units::base_16::bytes heap_lfh_bucket::get_bucket_granularity() const
    {
        return size_units::base_16::bytes{bucket_index_map.lower_bound(bucket_index())->second.granularity};
    }

    size_units::base_16::bytes heap_lfh_bucket::get_max_allocation_size() const
    {
        auto const& bucket_data = bucket_index_map.lower_bound(bucket_index());
        return size_units::base_16::bytes{bucket_data->second.base_size + bucket_data->second.granularity * (bucket_index() - bucket_data->second.offset + bucket_data->second.index_adjustment)};
    }
}
