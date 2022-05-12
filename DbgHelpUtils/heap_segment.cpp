#include "heap_segment.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_entry.h"
#include "heap_ucr_descriptor.h"
#include "list_entry_walker.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_segment::symbol_name = common_symbol_names::heap_segment_structure_symbol_name;

    heap_segment::heap_segment(nt_heap const& heap, uint64_t const heap_segment_address)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , heap_segment_address_{heap_segment_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_segment::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_segment::peb() const
    {
        return heap().peb();
    }

    uint32_t heap_segment::segment_flags() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_segment_segment_flags_field_data, common_symbol_names::heap_segment_segment_flags_field_symbol_name);
    }

    uint64_t heap_segment::base_address() const
    {
        return get_field_pointer(*this, cache_data_->heap_segment_base_address_field_data, common_symbol_names::heap_segment_base_address_field_symbol_name);
    }

    uint32_t heap_segment::number_of_pages() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_segment_number_of_pages_field_data, common_symbol_names::heap_segment_number_of_pages_field_symbol_name);
    }

    uint64_t heap_segment::first_entry() const
    {
        return get_field_pointer(*this, cache_data_->heap_segment_first_entry_field_data, common_symbol_names::heap_segment_first_entry_field_symbol_name);
    }

    uint64_t heap_segment::last_entry() const
    {
        return get_field_pointer(*this, cache_data_->heap_segment_last_entry_field_data, common_symbol_names::heap_segment_last_entry_field_symbol_name);
    }

    uint32_t heap_segment::number_of_uncommitted_pages() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_segment_number_of_un_committed_pages_field_data, common_symbol_names::heap_segment_number_of_un_committed_pages_field_symbol_name);
    }

    uint32_t heap_segment::number_of_uncommitted_ranges() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->heap_segment_number_of_un_committed_ranges_field_data, common_symbol_names::heap_segment_number_of_un_committed_ranges_field_symbol_name);
    }

    uint16_t heap_segment::segment_allocator_back_trace_index() const
    {
        return stream_utils::get_field_value<uint16_t>(*this, cache_data_->heap_segment_segment_allocator_back_trace_index_field_data, common_symbol_names::heap_segment_segment_allocator_back_trace_index_field_symbol_name);
    }

    std::experimental::generator<heap_entry> heap_segment::entries() const
    {
        auto const last_entry_address = last_entry();
        auto entry_address = heap_segment_address();

        std::map<uint64_t, heap_ucr_descriptor> uncommitted_range_values;
        for (auto const& uncommitted_range : uncommitted_ranges())
        {
            uncommitted_range_values.insert(std::make_pair(uncommitted_range.address(), uncommitted_range));
        }

        size_units::base_16::bytes previous_size{0};

        while(entry_address < last_entry_address)
        {
            if(auto const uncommitted_range = is_uncommitted_range(uncommitted_range_values, entry_address); uncommitted_range.has_value())
            {
                heap_entry entry{heap(), uncommitted_range.value().address(), static_cast<uint64_t>(uncommitted_range.value().size().count())};
                co_yield entry;
                entry_address += entry.size().count();
                previous_size = entry.size();
            }
            else
            {
                auto buffer = std::make_shared<uint8_t[]>(heap().granularity());
                if(auto stream = walker().get_process_memory_stream(entry_address, heap().granularity());
                    stream.eof() || stream.read(buffer.get(), heap().granularity()) != heap().granularity())
                {
                    co_yield heap_entry{heap(), entry_address, last_entry_address - entry_address};
                    break;
                }

                heap().decode_heap_entry(buffer.get());

                heap_entry entry{heap(), entry_address, std::move(buffer), previous_size};
                co_yield entry;

                entry_address += entry.size().count();
                previous_size = entry.size();
            }
        }

        if(entry_address == last_entry_address)
        {
            if(auto const uncommitted_range = is_uncommitted_range(uncommitted_range_values, entry_address); uncommitted_range.has_value())
            {
                heap_entry entry{heap(), uncommitted_range.value().address(), static_cast<uint64_t>(uncommitted_range.value().size().count())};
                co_yield entry;
            }
        }
    }

    std::experimental::generator<heap_ucr_descriptor> heap_segment::uncommitted_ranges() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{heap().cache(), walker(), stream_utils::get_field_address(*this, cache_data_->heap_segment_ucr_segment_list_field_data, common_symbol_names::heap_segment_ucr_segment_list_field_symbol_name), heap_ucr_descriptor::symbol_name, common_symbol_names::heap_ucr_descriptor_segment_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_ucr_descriptor{heap(), entry_address};
        }
    }

    void heap_segment::setup_globals(nt_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_segment_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);

            data.heap_segment_segment_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_segment_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_number_of_pages_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_number_of_pages_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_number_of_un_committed_pages_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_number_of_un_committed_pages_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_number_of_un_committed_ranges_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_number_of_un_committed_ranges_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_segment_allocator_back_trace_index_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_segment_allocator_back_trace_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_segment_base_address_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_base_address_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.heap_segment_first_entry_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_first_entry_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.heap_segment_last_entry_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_segment_symbol_type, symbol_name, common_symbol_names::heap_segment_last_entry_field_symbol_name, dbg_help::sym_tag_enum::PointerType);

            data.heap_segment_ucr_segment_list_field_data = data.heap_segment_symbol_type.get_field_in_type(symbol_name, common_symbol_names::heap_segment_ucr_segment_list_field_symbol_name);
        }
    }

    std::optional<heap_ucr_descriptor> heap_segment::is_uncommitted_range(std::map<uint64_t, heap_ucr_descriptor> const& ranges, uint64_t const heap_entry_address)
    {
        if(auto const it = ranges.find(heap_entry_address); it != ranges.end())
        {
            return it->second;
        }

        return std::nullopt;
    }
}
