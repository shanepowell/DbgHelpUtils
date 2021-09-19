#include "heap_entry.h"

#include <array>

#include "common_symbol_names.h"
#include "nt_heap.h"
#include "process_environment_block.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    std::wstring const& heap_entry::symbol_name = common_symbol_names::heap_entry_structure_symbol_name;

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset()}
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_user_address_{get_ust_user_address()}
    , unused_bytes_{get_unused_bytes()}
    , requested_size_{get_requested_size()}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_16::bytes{-1})}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer, uint16_t const block_size, LfhEntryType)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{block_size * heap.granularity()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_user_address_{get_ust_user_address()}
    , unused_bytes_{get_unused_bytes()}
    , requested_size_{get_requested_size()}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const end_address, std::shared_ptr<uint8_t[]> buffer, uint64_t const size, uint16_t const unused_bytes, VirtualAllocType)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{size}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , ust_user_address_{get_ust_user_address()}
    , unused_bytes_{unused_bytes}
    , requested_size_{get_virtual_alloc_requested_size(size, unused_bytes)}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_virtual_alloc_end_unused_bytes(end_address)}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_16::bytes{-1})}
    , is_virtual_alloc_{true}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer, size_units::base_16::bytes const previous_size)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_user_address_{get_ust_user_address()}
    , unused_bytes_{get_unused_bytes()}
    , requested_size_{get_requested_size()}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(previous_size)}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const uncommitted_size)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , size_{uncommitted_size}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const unknown_size, UnknownSizeType)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , flags_{FlagBusy}
    , size_{unknown_size}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_entry::peb() const
    {
        return heap().peb();
    }

    bool heap_entry::is_lfh_busy() const
    {
        if(peb().user_stack_db_enabled())
        {
            return raw_unused_bytes_ == LfhFlagUstBusy;
        }

        return unused_bytes_raw() != 0;
    }

    bool heap_entry::is_heap_allocation() const
    {
        return address() == heap().nt_heap_address();
    }

    bool heap_entry::get_is_valid(size_units::base_16::bytes const previous_size) const
    {
        if(peb().is_x86_target())
        {
            if(auto const* data = reinterpret_cast<uint8_t const*>(buffer_.get()); 
                (data[0] ^ data[1] ^ data[2] ^ data[3]) != 0x00)
            {
                return false;
            }
        }

        if(peb().is_x64_target())
        {
            if(auto const* data = reinterpret_cast<uint8_t const*>(buffer_.get());
                (data[0x08] ^ data[0x09] ^ data[0x0a] ^ data[0x0b]) != 0x00)
            {
                return false;
            }
        }

        return previous_size < size_units::base_16::bytes{0} || previous_size == previous_size_;
    }

    uint8_t heap_entry::get_flags()  const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t>(*this, common_symbol_names::heap_entry_flags_field_symbol_name, buffer_.get());
    }

    uint64_t heap_entry::get_size()  const
    {
        return static_cast<uint64_t>(stream_utils::get_field_value_from_buffer<uint16_t>(*this, common_symbol_names::heap_entry_size_field_symbol_name, buffer_.get())) * heap().granularity();
    }

    uint64_t heap_entry::get_previous_size() const
    {
        return static_cast<uint64_t>(stream_utils::get_field_value_from_buffer<uint16_t>(*this, common_symbol_names::heap_entry_previous_size_field_symbol_name, buffer_.get())) * heap().granularity();
    }

    uint8_t heap_entry::get_segment_offset() const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t>(*this, common_symbol_names::heap_entry_segment_offset_field_symbol_name, buffer_.get());
    }

    uint8_t heap_entry::get_raw_unused_bytes()  const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t>(*this, common_symbol_names::heap_entry_unused_bytes_field_symbol_name, buffer_.get());
    }

    size_units::base_16::bytes heap_entry::get_unused_bytes() const
    {
        size_units::base_16::bytes unused_bytes_data;
        if(ust_user_address_ != 0)
        {
            const auto unused_bytes_value = stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker(), heap_entry_symbol_type_, common_symbol_names::heap_entry_unused_bytes_length_field_symbol_name, ust_user_address_ - heap_entry_length_);
            unused_bytes_data = size_units::base_16::bytes{unused_bytes_value.value()};
        }
        else
        {
            unused_bytes_data = size_units::base_16::bytes{unused_bytes_raw()};
        }

        if(unused_bytes_data > size())
        {
            return size() - unused_bytes_data;
        }

        return unused_bytes_data;
    }

    size_units::base_16::bytes heap_entry::get_requested_size() const
    {
        size_units::base_16::bytes unused_bytes_data;
        if(ust_user_address_ != 0)
        {
            unused_bytes_data = unused_bytes();
        }
        else
        {
            unused_bytes_data = size_units::base_16::bytes{unused_bytes_raw()};
        }

        if(unused_bytes_data > size())
        {
            return unused_bytes_data - size();
        }

        return size() - unused_bytes_data;
    }

    uint64_t heap_entry::get_user_address() const
    {
        if(is_busy() && !is_heap_allocation())
        {
            if(ust_user_address_ != 0)
            {
                return get_ust_user_address();
            }

            return heap_entry_address_ + heap_entry_length_ + (is_front_padded() ? heap_entry_length_ : 0);
        }
        return 0;
    }

    size_units::base_16::bytes heap_entry::get_end_unused_bytes() const
    {
        if(user_address() == 0)
        {
            return size_units::base_16::bytes{0};
        }

        return size_units::base_16::bytes{(address() + size().count()) - (user_address() + user_requested_size().count())};
    }

    uint64_t heap_entry::get_ust_address() const
    {
        if(ust_user_address_ == 0 || is_heap_allocation() || !is_busy())
        {
            return 0;
        }

        auto const value = stream_utils::read_machine_size_field_value(peb(), heap_entry_address_ + heap_entry_length_);
        if(!value.has_value())
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: symbol " << symbol_name << " can't get ust address field data").str()};
        }

        return value.value();
    }

    uint64_t heap_entry::get_ust_user_address() const
    {
        if(is_heap_allocation())
        {
            return 0;
        }

        auto const min_ust_data_size = heap_entry_length_ + heap_entry_length_;
        auto const end_address = heap_entry_address_ + size().count();
        auto stream = walker().get_process_memory_stream(heap_entry_address_ + min_ust_data_size, size().count() - min_ust_data_size);

        std::array constexpr find_values =
        {
            static_cast<uint16_t>(0x0001),
            static_cast<uint16_t>(0x0000),
            std::numeric_limits<uint16_t>::max(),
            static_cast<uint16_t>(0x0502)
        };

        size_t index = 0;
        while(!stream.eof() && stream.current_address() < end_address)
        {
            uint16_t check;
            if(stream.read(&check, sizeof check) != sizeof check)
            {
                break;
            }

            if(find_values[index] == std::numeric_limits<uint16_t>::max() || find_values[index] == check)
            {
                ++index;
                if(index == find_values.size())
                {
                    return stream.current_address();
                }
            }
            else
            {
                index = 0;
            }
        }

        return 0;
    }

    std::vector<uint64_t> heap_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }

    size_units::base_16::bytes heap_entry::get_virtual_alloc_requested_size(uint64_t const size, uint16_t const unused_bytes)
    {
        return size_units::base_16::bytes{size - unused_bytes};
    }

    size_units::base_16::bytes heap_entry::get_virtual_alloc_end_unused_bytes(uint64_t const end_address) const
    {
        return size_units::base_16::bytes{end_address - (user_address() + user_requested_size().count())};
    }

    dbg_help::symbol_type_info heap_entry::get_heap_entry_symbol_type() const
    {
        return stream_utils::get_type(walker(), symbol_name);
    }

    size_t heap_entry::get_heap_entry_length() const
    {
        return stream_utils::get_type_length(heap_entry_symbol_type_, symbol_name);
    }

    uint64_t heap_entry::get_small_tag_index_offset() const
    {
        return stream_utils::get_field_offset(heap_entry_symbol_type_, symbol_name, common_symbol_names::heap_entry_small_tag_index_field_symbol_name);
    }
}
