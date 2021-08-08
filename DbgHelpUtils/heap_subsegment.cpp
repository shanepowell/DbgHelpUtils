#include "heap_subsegment.h"

#include <sstream>

#include "common_symbol_names.h"
#include "heap_entry.h"
#include "lfh_heap.h"
#include "nt_heap.h"
#include "stream_utils.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    heap_subsegment::heap_subsegment(lfh_heap const& heap, uint64_t const heap_subsegment_address, uint64_t const lfh_block_zone_size)
    : lfh_heap_{heap}
    , heap_subsegment_address_{heap_subsegment_address}
    , lfh_block_zone_size_{lfh_block_zone_size}
    , heap_subsegment_symbol_type_{stream_utils::get_type(lfh_heap_.walker(), common_symbol_names::heap_subsegment_structure_symbol_name)}
    , heap_user_data_header_symbol_type_{stream_utils::get_type(lfh_heap_.walker(), common_symbol_names::heap_user_data_header_structure_symbol_name)}
    {
        auto [entry_start_address, block_stride] = get_entry_data();
        entry_start_address_ = entry_start_address;
        block_stride_ = block_stride;
    }

    template <typename T>
    T heap_subsegment::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(lfh_heap_.walker(), heap_subsegment_symbol_type_, field_name, heap_subsegment_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint16_t heap_subsegment::block_size() const
    {
        return block_size_raw() * static_cast<uint16_t>(lfh_heap_.heap().granularity());
    }

    uint16_t heap_subsegment::block_size_raw() const
    {
        return get_field_value<uint16_t>(common_symbol_names::heap_subsegment_block_size_field_symbol_name);
    }

    uint16_t heap_subsegment::block_count() const
    {
        return get_field_value<uint16_t>(common_symbol_names::heap_subsegment_block_count_field_symbol_name);
    }

    std::experimental::generator<heap_entry> heap_subsegment::entries() const
    {
        auto const block_size_value = block_size_raw();
        auto const block_count_value = block_count();

        auto address = entry_start_address_;

        for(uint16_t index = 0; index < block_count_value; ++index)
        {
            auto buffer = std::make_shared<char[]>(lfh_heap_.heap().granularity());
            auto const* entry_data = lfh_heap_.walker().get_process_memory(address, lfh_heap_.heap().granularity());
            if(entry_data == nullptr)
            {
                break;
            }

            memcpy(buffer.get(), entry_data, lfh_heap_.heap().granularity());

            heap_entry entry{lfh_heap_.heap(), address, std::move(buffer), block_size_value, heap_entry::LfhEntryType{}};
            co_yield entry;

            address += block_stride_;
        }
    }

    std::tuple<uint64_t, uint16_t> heap_subsegment::get_entry_data() const
    {
        auto const block_size_value = block_size_raw();
        if(block_size_value == 0)
        {
            return std::make_tuple(0ULL, static_cast<unsigned short>(0));
        }

        auto const user_blocks_value = stream_utils::find_field_pointer_type_and_value_in_type(lfh_heap_.walker(), heap_subsegment_symbol_type_, common_symbol_names::heap_subsegment_user_blocks_field_symbol_name, heap_subsegment_address_);
        if(!user_blocks_value.has_value())
        {
            throw_cant_get_field_data(common_symbol_names::heap_subsegment_structure_symbol_name);
        }

        auto const user_blocks_address = user_blocks_value.value().second;
        if(user_blocks_address == 0)
        {
            return std::make_tuple(0ULL, static_cast<unsigned short>(0));
        }

        uint64_t address;
        uint16_t block_stride;
        if(auto const encoded_offsets = stream_utils::read_udt_value_in_type(lfh_heap_.walker(), heap_user_data_header_symbol_type_, common_symbol_names::heap_user_data_encoded_offsets_field_symbol_name, user_blocks_address);
            encoded_offsets.has_value())
        {
            if(auto const lfh_key = lfh_heap_.lfh_key(); lfh_key.has_value())
            {
                [[maybe_unused]] auto& [encoded_offsets_buffer, encoded_offsets_length, encoded_offsets_address] = encoded_offsets.value();

                if(encoded_offsets_length < sizeof(uint32_t))
                {
                    throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: Field " << common_symbol_names::heap_user_data_encoded_offsets_field_symbol_name << " length (" << encoded_offsets_length << ") to small").str()};
                }

                uint32_t encoded_offsets_value;
                memcpy(&encoded_offsets_value, encoded_offsets_buffer.get(), sizeof(uint32_t));

                auto const offsets = encoded_offsets_value ^ user_blocks_address ^ static_cast<uint32_t>(lfh_heap_.address()) ^ lfh_key.value();
                auto const first_allocation_offset = static_cast<uint16_t>(offsets);
                address = user_blocks_address + first_allocation_offset;
                block_stride = static_cast<uint16_t>(offsets >> 16);
            }
            else
            {
                throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: Failed to find expected lfh key").str()};
            }
        }
        else if(auto const first_allocation_offset = stream_utils::find_basic_type_field_value_in_type<uint16_t>(lfh_heap_.walker(), heap_user_data_header_symbol_type_, common_symbol_names::heap_user_data_first_allocation_offset_field_symbol_name, user_blocks_address); 
            first_allocation_offset.has_value())
        {
            address = user_blocks_address + first_allocation_offset.value();
            block_stride = block_size_value * static_cast<uint16_t>(lfh_heap_.heap().granularity());
        }
        else
        {
            address = user_blocks_address + lfh_block_zone_size_;
            block_stride = block_size_value * static_cast<uint16_t>(lfh_heap_.heap().granularity());
        }

        return std::make_tuple(address, block_stride);
    }

    void heap_subsegment::throw_cant_get_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_data(common_symbol_names::heap_subsegment_structure_symbol_name, field_name);
    }

    void heap_subsegment::throw_cant_get_user_data_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_data(common_symbol_names::heap_user_data_header_structure_symbol_name, field_name);
    }
}
