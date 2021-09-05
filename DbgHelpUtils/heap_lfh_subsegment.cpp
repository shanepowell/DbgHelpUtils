#include "heap_lfh_subsegment.h"

#include "common_symbol_names.h"
#include "heap_lfh_context.h"
#include "heap_lfh_entry.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_subsegment::symbol_name = common_symbol_names::heap_lfh_subsegment_structure_symbol_name;

    heap_lfh_subsegment::heap_lfh_subsegment(heap_lfh_context const& heap, const uint64_t heap_lfh_subsegment_address)
    : heap_{heap}
    , heap_lfh_subsegment_address_{heap_lfh_subsegment_address}
    , heap_lfh_subsegment_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_lfh_subsegment_encoded_offsets_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::heap_lfh_subsegment_encoded_offsets_structure_symbol_name)}
    , heap_lfh_subsegment_bitmap_offset_{stream_utils::get_field_offset(heap_lfh_subsegment_symbol_type_, symbol_name, common_symbol_names::heap_lfh_subsegment_block_bitmap_field_symbol_name)}
    {
        if(auto block_offsets = stream_utils::read_udt_value_in_type(walker(), heap_lfh_subsegment_symbol_type_, common_symbol_names::heap_lfh_subsegment_block_offsets_field_symbol_name, heap_lfh_subsegment_address);
            block_offsets.has_value())
        {
            block_offsets_ = std::move(std::get<0>(block_offsets.value()));

            // decode data
            auto const key = heap.heap().lfh_heap_key() ^ (static_cast<uint32_t>(heap_lfh_subsegment_address) >> 0xC);
            if(auto const data_length = std::get<1>(block_offsets.value()); data_length >= sizeof key)
            {
                auto* data = block_offsets_.get();
                auto* end_data = block_offsets_.get() + sizeof(key);
                auto const* key_data = reinterpret_cast<uint8_t const*>(&key);
                while(data != end_data)
                {
                    *data ^= *key_data;
                    ++data;
                    ++key_data;
                }
            }
        }
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_lfh_subsegment::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_subsegment::peb() const
    {
        return heap().peb();
    }

    uint16_t heap_lfh_subsegment::free_count() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_lfh_subsegment_free_count_field_symbol_name);
    }

    uint16_t heap_lfh_subsegment::block_count() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, common_symbol_names::heap_lfh_subsegment_block_count_field_symbol_name);
    }

    lfh_subsegment_location_utils::location heap_lfh_subsegment::location() const
    {
        return static_cast<lfh_subsegment_location_utils::location>(stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_subsegment_location_field_symbol_name));
    }

    uint8_t heap_lfh_subsegment::witheld_block_count() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_subsegment_witheld_block_count_field_symbol_name);
    }

    template <typename T>
    T heap_lfh_subsegment::get_field_value_from_block_offsets(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(heap_lfh_subsegment_encoded_offsets_symbol_type_, field_name, block_offsets_.get());
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::heap_lfh_subsegment_encoded_offsets_structure_symbol_name, field_name);
        }

        return value.value();
    }

    size_units::base_10::bytes heap_lfh_subsegment::block_size() const
    {
        return size_units::base_10::bytes{get_field_value_from_block_offsets<uint16_t>(common_symbol_names::heap_lfh_subsegment_encoded_offsets_block_size_field_symbol_name)};
    }

    uint8_t heap_lfh_subsegment::commit_unit_shift() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_subsegment_commit_unit_shift_field_symbol_name);
    }

    uint8_t heap_lfh_subsegment::commit_unit_count() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_subsegment_commit_unit_count_field_symbol_name);
    }

    std::experimental::generator<heap_lfh_entry> heap_lfh_subsegment::entries() const
    {
        auto const block_count_data = block_count();
        auto const block_size_data = block_size();
        auto block_address = heap_lfh_subsegment_address() + get_field_value_from_block_offsets<uint16_t>(common_symbol_names::heap_lfh_subsegment_encoded_offsets_first_block_offset_field_symbol_name);
        auto block_bitmap_address = heap_lfh_subsegment_address() + heap_lfh_subsegment_bitmap_offset_;
        uint32_t mask = 0x00;
        uint32_t data{};

        for(uint16_t index = 0; index < block_count_data; ++index)
        {
            if(!mask)
            {
                mask = 0x01;

                auto const data_value = stream_utils::read_field_value<uint32_t>(walker(), block_bitmap_address);
                if(!data_value.has_value())
                {
                    break;
                }

                block_bitmap_address += sizeof(uint32_t);
                data = data_value.value();
            }

            auto const is_allocated = (data & mask) == mask;
            mask <<= 1;
            auto const has_used_bytes = (data & mask) == mask;
            mask <<= 1;
            co_yield heap_lfh_entry{heap(), block_address, block_size_data, is_allocated, has_used_bytes};

            block_address += block_size_data.count();
        }
    }
}
