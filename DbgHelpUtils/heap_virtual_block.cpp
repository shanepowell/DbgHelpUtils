#include "heap_virtual_block.h"

#include "common_symbol_names.h"
#include "heap_entry.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    heap_virtual_block::heap_virtual_block(nt_heap const& heap, uint64_t const heap_virtual_block_address)
    : heap_{heap}
    , heap_virtual_block_address_(heap_virtual_block_address)
    , heap_virtual_block_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::heap_virtual_alloc_entry_structure_symbol_name)}
    , data_address_{heap_virtual_block_address + stream_utils::get_type_length(heap_virtual_block_symbol_type_, common_symbol_names::heap_virtual_alloc_entry_structure_symbol_name)}
    , busy_block_offset_{stream_utils::get_field_offset(heap_virtual_block_symbol_type_, common_symbol_names::heap_virtual_alloc_entry_structure_symbol_name, common_symbol_names::heap_virtual_alloc_entry_busy_block_field_symbol_name)}
    {
    }

    uint64_t heap_virtual_block::get_machine_size_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_machine_size_field_value(heap_, heap_virtual_block_symbol_type_, field_name, heap_virtual_block_address_);
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::heap_virtual_alloc_entry_structure_symbol_name, field_name);
        }

        return value.value();
    }

    size_units::base_10::bytes heap_virtual_block::reserved() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_virtual_alloc_entry_reserve_size_field_symbol_name) };
    }

    size_units::base_10::bytes heap_virtual_block::committed() const
    {
        return size_units::base_10::bytes{ get_machine_size_field_value(common_symbol_names::heap_virtual_alloc_entry_commit_size_field_symbol_name) };
    }

    std::experimental::generator<heap_entry> heap_virtual_block::entries() const
    {
        auto const committed_size = committed();
        if(auto const reserved_size = reserved(); committed_size > size_units::base_10::bytes{0})
        {
            auto const entry_address = descriptor_address() + busy_block_offset_;
            auto const last_address = address() + committed_size.count();
            auto const last_committed_address = address() + reserved_size.count();

            auto buffer = std::make_shared<char[]>(heap_.granularity());
            if(auto const* entry_data = heap_.walker().get_process_memory(entry_address, heap_.granularity()); entry_data == nullptr)
            {
                co_yield heap_entry{heap_, entry_address, last_address - entry_address};
            }
            else
            {
                heap_.decode_heap_entry(entry_data, buffer.get());

                uint16_t unused_bytes{0};
                if(heap_.is_x86_target())
                {
                    memcpy(&unused_bytes, buffer.get(), sizeof(uint16_t));
                }
                if(heap_.is_x64_target())
                {
                    memcpy(&unused_bytes, buffer.get() + 8, sizeof(uint16_t));
                }

                if(unused_bytes > committed_size.count())
                {
                    co_yield heap_entry{heap_, entry_address, last_address -  entry_address, heap_entry::UnknownSizeType{}};
                }
                else
                {
                    co_yield heap_entry{heap_, entry_address, last_address, std::move(buffer), static_cast<uint64_t>(committed_size.count()), unused_bytes, heap_entry::VirtualAllocType{} };
                }
            }

            if(last_address < last_committed_address)
            {
                co_yield heap_entry{heap_, last_address, last_committed_address -  last_address};
            }
        }
        else if(reserved_size > size_units::base_10::bytes{0})
        {
            co_yield heap_entry{heap_, address(), static_cast<uint64_t>(reserved_size.count())};
        }
    }
}
