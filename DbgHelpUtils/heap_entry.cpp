#include "heap_entry.h"

#include "common_symbol_names.h"
#include "nt_heap.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<char[]> buffer)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , is_valid_ust_area_{is_valid_ust_area()}
    , unused_bytes_{get_unused_bytes()}
    , requested_size_{get_requested_size()}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_10::bytes{-1})}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<char[]> buffer, uint16_t const block_size, LfhEntryType)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{block_size * heap_.granularity()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , is_valid_ust_area_{is_valid_ust_area()}
    , unused_bytes_{get_unused_bytes()}
    , requested_size_{get_requested_size()}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const end_address, std::shared_ptr<char[]> buffer, uint64_t const size, uint16_t const unused_bytes, VirtualAllocType)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{size}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , is_valid_ust_area_{is_valid_ust_area()}
    , unused_bytes_{unused_bytes}
    , requested_size_{get_virtual_alloc_requested_size(size, unused_bytes)}
    , user_address_{get_user_address()}
    , end_unused_bytes_{get_virtual_alloc_end_unused_bytes(end_address)}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_10::bytes{-1})}
    , is_virtual_alloc_{true}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<char[]> buffer, size_units::base_10::bytes const previous_size)
    : heap_{heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , small_tag_index_offset_{get_small_tag_index_offset() }
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , is_valid_ust_area_{is_valid_ust_area()}
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

    template <typename T>
    T heap_entry::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(heap_entry_symbol_type_, field_name, buffer_.get());
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::heap_entry_structure_symbol_name, field_name);
        }

        return value.value();
    }

    bool heap_entry::is_lfh_busy() const
    {
        if(heap().peb().user_stack_db_enabled())
        {
            return raw_unused_bytes_ == LfhFlagUstBusy;
        }

        return unused_bytes_raw() != 0;
    }

    bool heap_entry::is_heap_allocation() const
    {
        return address() == heap().nt_heap_address();
    }

    bool heap_entry::get_is_valid(size_units::base_10::bytes const previous_size) const
    {
        if(heap().peb().is_x86_target())
        {
            if(auto const* data = reinterpret_cast<uint8_t const*>(buffer_.get()); 
                (data[0] ^ data[1] ^ data[2] ^ data[3]) != 0x00)
            {
                return false;
            }
        }

        if(heap().peb().is_x64_target())
        {
            if(auto const* data = reinterpret_cast<uint8_t const*>(buffer_.get());
                (data[0x08] ^ data[0x09] ^ data[0x0a] ^ data[0x0b]) != 0x00)
            {
                return false;
            }
        }

        return previous_size < size_units::base_10::bytes{0} || previous_size == previous_size_;
    }

    uint8_t heap_entry::get_flags()  const
    {
        return get_field_value<uint8_t>(common_symbol_names::heap_entry_flags_field_symbol_name);
    }

    uint64_t heap_entry::get_size()  const
    {
        return static_cast<uint64_t>(get_field_value<uint16_t>(common_symbol_names::heap_entry_size_field_symbol_name)) * heap().granularity();
    }

    uint64_t heap_entry::get_previous_size() const
    {
        return static_cast<uint64_t>(get_field_value<uint16_t>(common_symbol_names::heap_entry_previous_size_field_symbol_name)) * heap().granularity();
    }

    uint8_t heap_entry::get_segment_offset() const
    {
        return get_field_value<uint8_t>(common_symbol_names::heap_entry_segment_offset_field_symbol_name);
    }

    uint8_t heap_entry::get_raw_unused_bytes()  const
    {
        return get_field_value<uint8_t>(common_symbol_names::heap_entry_unused_bytes_field_symbol_name);
    }

    bool heap_entry::is_valid_ust_area() const
    {
        if(heap().peb().user_stack_db_enabled() && is_busy())
        {
            const auto unused_bytes_value = stream_utils::find_basic_type_field_value_in_type<uint16_t>(heap().walker(), heap_entry_symbol_type_, common_symbol_names::heap_entry_unused_bytes_length_field_symbol_name, get_ust_data_heap_entry_address());
            if(!unused_bytes_value.has_value())
            {
                throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: symbol " << common_symbol_names::heap_entry_structure_symbol_name << " can't get ust extra field data").str()};
            }

            auto const unused_bytes_data = size_units::base_10::bytes{unused_bytes_value.value()};
            return unused_bytes_data <= size();
        }

        return false;
    }

    size_units::base_10::bytes heap_entry::get_unused_bytes() const
    {
        size_units::base_10::bytes unused_bytes_data;
        if(is_valid_ust_area_)
        {
            const auto unused_bytes_value = stream_utils::find_basic_type_field_value_in_type<uint16_t>(heap().walker(), heap_entry_symbol_type_, common_symbol_names::heap_entry_unused_bytes_length_field_symbol_name, get_ust_data_heap_entry_address());
            unused_bytes_data = size_units::base_10::bytes{unused_bytes_value.value()};
        }
        else
        {
            unused_bytes_data = size_units::base_10::bytes{unused_bytes_raw()};
        }

        if(unused_bytes_data > size())
        {
            return size() - unused_bytes_data;
        }

        return unused_bytes_data;
    }

    size_units::base_10::bytes heap_entry::get_requested_size() const
    {
        size_units::base_10::bytes unused_bytes_data;
        if(is_valid_ust_area_)
        {
            unused_bytes_data = unused_bytes();
        }
        else
        {
            unused_bytes_data = size_units::base_10::bytes{unused_bytes_raw()};
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
            if(is_valid_ust_area_)
            {
                return heap_entry_address_ + heap_entry_length_ + get_ust_data_size();
            }

            return heap_entry_address_ + heap_entry_length_ + (is_front_padded() ? heap_entry_length_ : 0);
        }
        return 0;
    }

    size_units::base_10::bytes heap_entry::get_end_unused_bytes() const
    {
        if(user_address() == 0)
        {
            return size_units::base_10::bytes{0};
        }

        return size_units::base_10::bytes{(address() + size().count()) - (user_address() + requested_size().count())};
    }

    uint64_t heap_entry::get_ust_address() const
    {
        if(!is_valid_ust_area_ || is_heap_allocation() || !is_busy())
        {
            return 0;
        }

        auto const value = stream_utils::read_machine_size_field_value(heap_.peb(), heap_entry_address_ + heap_entry_length_);
        if(!value.has_value())
        {
            throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: symbol " << common_symbol_names::heap_entry_structure_symbol_name << " can't get ust address field data").str()};
        }

        return value.value();
    }

    uint64_t heap_entry::get_ust_data_size() const
    {
        if(heap().peb().is_x86_target())
        {
            return heap_entry_length_ + 0x02;
        }
        if(heap().peb().is_x64_target())
        {
            return heap_entry_length_ + 0x10;
        }

        return 0;
    }

    uint64_t heap_entry::get_ust_data_heap_entry_address() const
    {
        return heap_entry_address_ + heap_entry_length_ + get_ust_data_size() - heap_entry_length_;
    }

    std::vector<uint64_t> heap_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(heap().peb(), ust_address());
    }

    size_units::base_10::bytes heap_entry::get_virtual_alloc_requested_size(uint64_t const size, uint16_t const unused_bytes)
    {
        return size_units::base_10::bytes{size - unused_bytes};
    }

    size_units::base_10::bytes heap_entry::get_virtual_alloc_end_unused_bytes(uint64_t const end_address) const
    {
        return size_units::base_10::bytes{end_address - (user_address() + requested_size().count())};
    }

    dbg_help::symbol_type_info heap_entry::get_heap_entry_symbol_type() const
    {
        return stream_utils::get_type(heap().walker(), common_symbol_names::heap_entry_structure_symbol_name);
    }

    uint64_t heap_entry::get_heap_entry_length() const
    {
        return stream_utils::get_type_length(heap_entry_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name);
    }

    uint64_t heap_entry::get_small_tag_index_offset() const
    {
        return stream_utils::get_field_offset(heap_entry_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name, common_symbol_names::heap_entry_small_tag_index_field_symbol_name);
    }
}
