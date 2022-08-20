#include "heap_entry.h"

#include <array>
#include <format>

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "nt_heap.h"
#include "process_environment_block.h"
#include "stream_hex_dump.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    namespace
    {
        auto constexpr ust_header_size_x86 = 0x18LL;
        auto constexpr ust_header_size_x64 = 0x30LL;
    }

    std::wstring const& heap_entry::symbol_name = common_symbol_names::heap_entry_structure_symbol_name;

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , small_tag_index_{get_small_tag_index()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_header_address_{get_ust_header_address()}
    , user_address_{get_user_address()}
    , unused_bytes_{get_unused_bytes(unused_bytes_method_, eod_, ust_end_gap_length_)}
    , requested_size_{get_requested_size(requested_size_method_)}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_16::bytes{-1})}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer, uint16_t const block_size, LfhEntryType)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , flags_{get_flags()}
    , size_{block_size * heap.granularity()}
    , lfh_index_{get_lfh_index()}
    , small_tag_index_{get_small_tag_index()}
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_header_address_{get_ust_header_address()}
    , user_address_{get_user_address()}
    , unused_bytes_{get_unused_bytes(unused_bytes_method_, eod_, ust_end_gap_length_)}
    , requested_size_{get_requested_size(requested_size_method_)}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{is_lfh_entry()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const end_address, std::shared_ptr<uint8_t[]> buffer, uint64_t const size, uint16_t const unused_bytes, VirtualAllocType)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , flags_{get_flags()}
    , size_{size}
    , previous_size_{get_previous_size()}
    , small_tag_index_{get_small_tag_index() }
    , segment_offset_{get_segment_offset()}
    , ust_header_address_{get_ust_header_address()}
    , user_address_{get_user_address()}
    , unused_bytes_{unused_bytes}
    , requested_size_{get_virtual_alloc_requested_size(size, unused_bytes)}
    , end_unused_bytes_{get_virtual_alloc_end_unused_bytes(end_address)}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(size_units::base_16::bytes{-1})}
    , is_virtual_alloc_{true}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, std::shared_ptr<uint8_t[]> buffer, size_units::base_16::bytes const previous_size)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , buffer_{std::move(buffer)}
    , flags_{get_flags()}
    , size_{get_size()}
    , previous_size_{get_previous_size()}
    , small_tag_index_{get_small_tag_index() }
    , segment_offset_{get_segment_offset()}
    , raw_unused_bytes_{get_raw_unused_bytes()}
    , ust_header_address_{get_ust_header_address()}
    , user_address_{get_user_address()}
    , unused_bytes_{get_unused_bytes(unused_bytes_method_, eod_, ust_end_gap_length_)}
    , requested_size_{get_requested_size(requested_size_method_)}
    , end_unused_bytes_{get_end_unused_bytes()}
    , ust_address_{get_ust_address()}
    , is_valid_{get_is_valid(previous_size)}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const uncommitted_size)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , size_{uncommitted_size}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const uncommitted_size, VirtualAllocType)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , size_{uncommitted_size}
    , is_virtual_alloc_{true}
    {
        validate_entry();
    }

    heap_entry::heap_entry(nt_heap const& heap, uint64_t const heap_entry_address, uint64_t const unknown_size, UnknownSizeType)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_entry_address_{heap_entry_address}
    , flags_{FlagBusy}
    , size_{unknown_size}
    , is_virtual_alloc_{true}
    {
        validate_entry();
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_entry::peb() const
    {
        return heap().peb();
    }

    size_units::base_16::bytes heap_entry::ust_unused_bytes_raw() const
    {
        if(ust_header_address() == 0)
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: call to ust_unused_bytes_raw on a non UST heap entry")};
        }

        return size_units::base_16::bytes{stream_utils::find_basic_type_field_value_in_type<uint16_t>(walker(), cache_data_->heap_entry_unused_bytes_length_field_data, user_address() - cache_data_->heap_entry_length).value_or(0)};
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
        auto const size_data = buffer_.get() + cache_data_->heap_entry_size_field_data.field_offset;
        if(auto const checksum = size_data[0] ^ size_data[1] ^ size_data[2];
            small_tag_index() != checksum)
        {
            return false;
        }

        if(is_lfh_entry())
        {
            return false;
        }

        return previous_size < size_units::base_16::bytes{0} || previous_size == previous_size_;
    }

    uint8_t heap_entry::get_flags()  const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t, heap_entry>(cache_data_->heap_entry_flags_field_data, common_symbol_names::heap_entry_flags_field_symbol_name, buffer_.get());
    }

    uint64_t heap_entry::get_size()  const
    {
        auto const block_size{static_cast<uint64_t>(stream_utils::get_field_value_from_buffer<uint16_t, heap_entry>(cache_data_->heap_entry_size_field_data, common_symbol_names::heap_entry_size_field_symbol_name, buffer_.get()))};
        return block_size * heap().granularity();
    }

    uint64_t heap_entry::get_previous_size() const
    {
        auto const previous_size{static_cast<uint64_t>(stream_utils::get_field_value_from_buffer<uint16_t, heap_entry>(cache_data_->heap_entry_previous_size_field_data, common_symbol_names::heap_entry_previous_size_field_symbol_name, buffer_.get()))};
        return previous_size * heap().granularity();
    }

    uint16_t heap_entry::get_lfh_index() const
    {
        // index is only the first byte, the second byte is unused
        auto const index{(stream_utils::get_field_value_from_buffer<uint16_t, heap_entry>(cache_data_->heap_entry_previous_size_field_data, common_symbol_names::heap_entry_previous_size_field_symbol_name, buffer_.get()))};
        return index >> 0x08;
    }

    uint8_t heap_entry::get_segment_offset() const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t, heap_entry>(cache_data_->heap_entry_segment_offset_field_data, common_symbol_names::heap_entry_segment_offset_field_symbol_name, buffer_.get());
    }

    uint8_t heap_entry::get_small_tag_index() const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t, heap_entry>(cache_data_->heap_entry_small_tag_index_field_data, common_symbol_names::heap_entry_small_tag_index_field_symbol_name, buffer_.get());
    }

    uint8_t heap_entry::get_raw_unused_bytes()  const
    {
        return stream_utils::get_field_value_from_buffer<uint8_t, heap_entry>(cache_data_->heap_entry_unused_bytes_field_data, common_symbol_names::heap_entry_unused_bytes_field_symbol_name, buffer_.get());
    }

    size_units::base_16::bytes heap_entry::get_unused_bytes(unused_bytes_type& unused_bytes_method, std::optional<size_units::base_16::bytes>& eod, std::optional<size_units::base_16::bytes>& ust_end_gap_length) const
    {
        using namespace size_units::base_16;
        bytes constexpr offset{0x8};
        bytes unused_bytes_data;
        bytes ust_unused_bytes_data{};

        auto const data_area_size = data_area_max_size();

        if(auto const eod_data = stream_utils::read_machine_size_field_value(peb(), user_address() + data_area_size.count() - peb().machine_pointer_size());
            eod_data.has_value())
        {
            eod = std::optional{bytes{static_cast<int64_t>(eod_data.value())}};
        }

        if(ust_header_address() != 0)
        {
            auto const end_gap_offset{peb().is_x64_target() ? 0x28 : 0x18};
            auto const end_gap_length = stream_utils::read_field_value<uint16_t>(walker(), ust_header_address() + sizeof(uint32_t));
            if(end_gap_length.has_value())
            {
                ust_end_gap_length = bytes{end_gap_length.value()};
            }

            if(end_gap_length.has_value() && end_gap_length.value() >= end_gap_offset)
            {
                auto const gap = end_gap_length.value() - end_gap_offset;
                unused_bytes_method = unused_bytes_type::unused_bytes_ust_header_gap;
                return bytes{gap};
            }

            auto const ust_header_size = bytes{user_address() - address()};
            ust_unused_bytes_data = unused_bytes_data = ust_unused_bytes_raw();

            if(unused_bytes_data >= ust_header_size)
            {
                unused_bytes_data -= ust_header_size;
            }
            unused_bytes_data += offset;
        }
        else
        {
            unused_bytes_data = bytes{unused_bytes_raw()};
        }

        if(unused_bytes_data == bytes{0x3F} && eod.has_value() && unused_bytes_data >= offset && eod.value() < size())
        {
            if(auto const request_size = size() - eod.value();
                data_area_size >= request_size)
            {
                unused_bytes_method = unused_bytes_type::unused_bytes_eod_size_offset;
                unused_bytes_data = data_area_size - request_size;
                return unused_bytes_data;
            }
        }

        if(unused_bytes_data > data_area_size)
        {
            auto const rv = unused_bytes_data - data_area_size;
            if(rv > data_area_size)
            {
                unused_bytes_method = unused_bytes_type::unused_bytes_gt_data_area_size_gt_data_area_size;
                auto const temp  = unused_bytes_data % data_area_size;
                return temp;
            }

            if(rv + offset > data_area_size)
            {
                unused_bytes_method = unused_bytes_type::unused_bytes_gt_data_area_size;
                return rv;
            }

            unused_bytes_method = unused_bytes_type::unused_bytes_gt_data_area_size_plus_offset;
            if(peb().is_x64_target() && rv <= offset)
            {
                unused_bytes_method = unused_bytes_type::unused_bytes_gt_data_area_size_offset_diff;
                return data_area_size - (offset - rv);
            }
            return rv + offset;
        }

        if(unused_bytes_data >= offset)
        {
            unused_bytes_data = unused_bytes_data - offset;

            if(ust_unused_bytes_data.count() > 0 && size() >= ust_unused_bytes_data)
            {
                
                if(auto const data_size = size() - ust_unused_bytes_data;
                    data_area_size >= data_size)
                {
                    unused_bytes_method = unused_bytes_type::size_gte_ust_unused_bytes_gte_data_area_size;
                    return data_area_size - data_size;
                }
            }

            unused_bytes_method = unused_bytes_type::unused_bytes_lte_data_area_size_minus_offset;
            return unused_bytes_data;
        }

        unused_bytes_method = unused_bytes_type::unused_bytes_lte_data_area_size_plus_offset;
        return unused_bytes_data + offset;
    }

    size_units::base_16::bytes heap_entry::get_requested_size(user_requested_size_type& requested_size_method) const
    {
        using namespace size_units::base_16;
        bytes const unused_bytes_data{unused_bytes()};

        auto const data_area_size = data_area_max_size();

        bytes requested_size;
        if(unused_bytes_data > size())
        {
            requested_size_method = user_requested_size_type::raw_gt_size;
            requested_size = unused_bytes_data - size();

            if(requested_size > size())
            {
                requested_size_method = user_requested_size_type::raw_gt_size_gt_size;
                requested_size  = unused_bytes_data % size();
            }
        }
        else if(unused_bytes_data <= data_area_size)
        {
            requested_size_method = user_requested_size_type::raw_lte_size;
            requested_size = data_area_size - unused_bytes_data;
        }
        else
        {
            requested_size_method = user_requested_size_type::bad_unused_size;
            requested_size = bytes{0};
        }

        return requested_size;
    }

    uint64_t heap_entry::get_user_address() const
    {
        if(is_busy() && !is_heap_allocation())
        {
            if(ust_header_address() != 0)
            {
                return address() + (peb().is_x64_target() ? ust_header_size_x64 : ust_header_size_x86);
            }

            return address() + cache_data_->heap_entry_length + front_padding().count();
        }
        return 0;
    }

    size_units::base_16::bytes heap_entry::get_end_unused_bytes() const
    {
        using namespace size_units::base_16;
        if(user_address() == 0)
        {
            return bytes{0};
        }

        return bytes{(address() + size().count()) - (user_address() + user_requested_size().count())};
    }

    uint64_t heap_entry::get_ust_address() const
    {
        if(!ust_address_stack_trace::has_ust_setup(peb()) || ust_header_address() == 0 || is_heap_allocation() || !is_busy())
        {
            return 0;
        }

        auto const value = stream_utils::read_machine_size_field_value(peb(), address() + cache_data_->heap_entry_length);
        if(!value.has_value())
        {
            throw exceptions::wide_runtime_error{std::format(L"Error: symbol {} can't get ust address field data", symbol_name)};
        }

        return value.value();
    }

    uint64_t heap_entry::get_ust_header_address() const
    {
        if(!ust_address_stack_trace::has_ust_setup(peb()) || is_heap_allocation())
        {
            return 0;
        }

        auto const min_ust_data_size = cache_data_->heap_entry_length + cache_data_->heap_entry_length;
        auto const ust_header_size = peb().is_x64_target() ? ust_header_size_x64 : ust_header_size_x86;
        auto stream = walker().get_process_memory_stream(address() + min_ust_data_size, ust_header_size);

        std::array constexpr find_values =
        {
            static_cast<uint16_t>(0x0001),
            static_cast<uint16_t>(0x0000),
            std::numeric_limits<uint16_t>::max(),
            static_cast<uint16_t>(0x0502)
        };

        while(!stream.eof())
        {
            if (stream.find_pattern<uint16_t>([&find_values](uint16_t const data, size_t const found_index, size_t&)
                {
                    return find_values[found_index] == std::numeric_limits<uint16_t>::max() || find_values[found_index] == data;
                }
                , [&find_values](size_t const found_index) { return found_index == find_values.size(); }))
            {
                return stream.current_address() - (find_values.size() * sizeof(uint16_t));
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

    size_units::base_16::bytes heap_entry::data_area_max_size() const
    {
        if(ust_header_address() != 0)
        {
            return size_units::base_16::bytes{ address() + size().count() + cache_data_->previous_block_private_data_size - user_address() };
        }

        return size_units::base_16::bytes{ size().count() + cache_data_->previous_block_private_data_size - cache_data_->heap_entry_length - front_padding().count() };
    }

    void heap_entry::validate_entry() const
    {
        if(user_address() + user_requested_size().count() > address() + size().count() + cache_data_->previous_block_private_data_size || user_requested_size().count() < 0)
        {
            throw exceptions::wide_runtime_error
                {
                    std::format(L"Error: heap entry has a invalid setup entry[{0} - {1}]({2}), user[{3} - {4}]({5})"
                        , stream_hex_dump::to_hex(address())
                        , stream_hex_dump::to_hex(address() + size().count())
                        , stream_hex_dump::to_hex(size().count())
                        , stream_hex_dump::to_hex(user_address())
                        , stream_hex_dump::to_hex(user_address() + user_requested_size().count())
                        , stream_hex_dump::to_hex(user_requested_size().count()))
                };
        }
    }

    void heap_entry::setup_globals(nt_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_entry_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_entry_length = stream_utils::get_type_length(data.heap_entry_symbol_type, symbol_name);
            data.heap_entry_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_previous_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_previous_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_segment_offset_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_segment_offset_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_small_tag_index_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_small_tag_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_unused_bytes_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_unused_bytes_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_entry_unused_bytes_length_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_entry_symbol_type, symbol_name, common_symbol_names::heap_entry_unused_bytes_length_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.previous_block_private_data_size = data.heap_entry_size_field_data.field_offset;
        }
    }
}
