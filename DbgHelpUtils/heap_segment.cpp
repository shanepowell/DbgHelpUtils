#include "heap_segment.h"

#include "common_symbol_names.h"
#include "heap_entry.h"
#include "heap_ucr_descriptor.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    heap_segment::heap_segment(nt_heap const& heap, uint64_t const heap_segment_address)
    : heap_{heap}
    , heap_segment_address_{heap_segment_address}
    , heap_segment_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::heap_segment_structure_symbol_name)}
    , heap_entry_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::heap_entry_structure_symbol_name)}
    , heap_ucr_descriptor_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::heap_ucr_descriptor_structure_symbol_name)}
    , list_entry_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::list_entry_structure_symbol_name)}
    , segment_entry_offset_{get_segment_entry_offset(heap_ucr_descriptor_symbol_type_)}
    {
    }

    template <typename T>
    T heap_segment::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(heap_.walker(), heap_segment_symbol_type_, field_name, heap_segment_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint32_t heap_segment::segment_flags() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_segment_segment_flags_field_symbol_name);
    }

    uint64_t heap_segment::base_address() const
    {
        return get_field_pointer(common_symbol_names::heap_segment_base_address_field_symbol_name);
    }

    uint32_t heap_segment::number_of_pages() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_segment_number_of_pages_field_symbol_name);
    }

    uint64_t heap_segment::first_entry() const
    {
        return get_field_pointer(common_symbol_names::heap_segment_first_entry_field_symbol_name);
    }

    uint64_t heap_segment::last_entry() const
    {
        return get_field_pointer(common_symbol_names::heap_segment_last_entry_field_symbol_name);
    }

    uint32_t heap_segment::number_of_uncommitted_pages() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_segment_number_of_un_committed_pages_field_symbol_name);
    }

    uint32_t heap_segment::number_of_uncommitted_ranges() const
    {
        return get_field_value<uint32_t>(common_symbol_names::heap_segment_number_of_un_committed_ranges_field_symbol_name);
    }

    uint16_t heap_segment::segment_allocator_back_trace_index() const
    {
        return get_field_value<uint16_t>(common_symbol_names::heap_segment_segment_allocator_back_trace_index_field_symbol_name);
    }

    std::experimental::generator<heap_entry> heap_segment::entries() const
    {
        auto const last_entry_address = last_entry();
        auto entry_address = heap_segment_address_;

        std::vector<heap_ucr_descriptor> ranges;
        ranges.reserve(number_of_uncommitted_ranges());
        for (auto const& uncommitted_range : uncommitted_ranges())
        {
            ranges.push_back(uncommitted_range);
        }

        size_units::base_10::bytes previous_size{0};

        while(entry_address < last_entry_address)
        {
            if(auto const uncommitted_range = is_uncommitted_range(ranges, entry_address); uncommitted_range.has_value())
            {
                heap_entry entry{heap_, uncommitted_range.value().address(), static_cast<uint64_t>(uncommitted_range.value().size().count())};
                co_yield entry;
                entry_address += entry.size().count();
                previous_size = entry.size();
            }
            else
            {
                auto buffer = std::make_shared<char[]>(heap_.granularity());
                auto const* entry_data = heap_.walker().get_process_memory(entry_address, heap_.granularity());
                if(entry_data == nullptr)
                {
                    co_yield heap_entry{heap_, entry_address, last_entry_address - entry_address};
                    break;
                }

                heap_.decode_heap_entry(entry_data, buffer.get());

                heap_entry entry{heap_, entry_address, std::move(buffer), previous_size};
                co_yield entry;

                entry_address += entry.size().count();
                previous_size = entry.size();
            }
        }

        if(entry_address == last_entry_address)
        {
            if(auto const uncommitted_range = is_uncommitted_range(ranges, entry_address); uncommitted_range.has_value())
            {
                heap_entry entry{heap_, uncommitted_range.value().address(), static_cast<uint64_t>(uncommitted_range.value().size().count())};
                co_yield entry;
            }
        }
    }

    std::experimental::generator<heap_ucr_descriptor> heap_segment::uncommitted_ranges() const
    {
        auto const start = get_field_address(common_symbol_names::heap_segment_ucr_segment_list_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::heap_segment_ucr_segment_list_flink_field_symbol_name);

        while(flink != start)
        {
            co_yield heap_ucr_descriptor{heap_, flink - segment_entry_offset_};

            flink = stream_utils::get_field_pointer(heap_.walker(), flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::optional<heap_ucr_descriptor> heap_segment::is_uncommitted_range(std::vector<heap_ucr_descriptor> const& ranges, uint64_t const heap_entry_address)
    {
        if(auto const it = std::ranges::find_if(ranges, [heap_entry_address](heap_ucr_descriptor const& range) { return range.address() == heap_entry_address; });
            it != ranges.end())
        {
            return *it;
        }

        return std::nullopt;
    }

    uint64_t heap_segment::get_field_pointer(std::wstring const& field_name) const
    {
        return stream_utils::get_field_pointer(heap_.walker(), heap_segment_address_, heap_segment_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name, field_name);
    }

    uint64_t heap_segment::get_field_address(std::wstring const& field_name) const
    {
        return heap_segment_address_ + stream_utils::get_field_offset(heap_segment_symbol_type_, common_symbol_names::heap_entry_structure_symbol_name, field_name);
    }


    uint64_t heap_segment::get_segment_entry_offset(dbg_help::symbol_type_info const& heap_ucr_descriptor_symbol_type)
    {
        return stream_utils::get_field_offset(heap_ucr_descriptor_symbol_type, common_symbol_names::heap_ucr_descriptor_structure_symbol_name, common_symbol_names::heap_ucr_descriptor_segment_entry_field_symbol_name);
    }

    void heap_segment::throw_cant_get_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_is_null(common_symbol_names::heap_segment_structure_symbol_name, field_name);
    }
}
