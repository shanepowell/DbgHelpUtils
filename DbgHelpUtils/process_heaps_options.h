#pragma once

#include "block_range_match_result.h"
#include "heap_node_type.h"
#include "segment_heap_options.h"
#include "size_units.h"

namespace dlg_help_utils::heap
{
    class crt_entry;

    enum class segment_heap_front_padding_options_type : uint8_t
    {
        auto_padding_detect,
        padding_enabled,
        padding_disabled
    };

    class process_heaps_options
    {
    public:
        [[nodiscard]] bool no_filter_heap_entries() const { return no_filter_heap_entries_; }
        [[nodiscard]] bool& no_filter_heap_entries() { return no_filter_heap_entries_; }

        [[nodiscard]] bool no_mark_global_variable_data_pointers_as_system() const { return no_mark_global_variable_data_pointers_as_system_; }
        [[nodiscard]] bool& no_mark_global_variable_data_pointers_as_system() { return no_mark_global_variable_data_pointers_as_system_; }

        [[nodiscard]] bool no_mark_all_crt_entries_as_system() const { return no_mark_all_crt_entries_as_system_; }
        [[nodiscard]] bool& no_mark_all_crt_entries_as_system() { return no_mark_all_crt_entries_as_system_; }

        [[nodiscard]] segment_heap_front_padding_options_type segment_heap_front_padding_options() const { return segment_heap_front_padding_options_; }
        [[nodiscard]] segment_heap_front_padding_options_type& segment_heap_front_padding_options() { return segment_heap_front_padding_options_; }

        [[nodiscard]] segment_heap_options segment_heap_options() const
        {
            switch(segment_heap_front_padding_options())
            {
            case segment_heap_front_padding_options_type::auto_padding_detect:
                return {0, 19041};

            case segment_heap_front_padding_options_type::padding_enabled:
                return {0, std::numeric_limits<uint16_t>::max()};

            case segment_heap_front_padding_options_type::padding_disabled:
                return {std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max()};
            }

            return {std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max()};
        }

        [[nodiscard]] std::vector<std::wstring> const& filtered_system_types() const { return filtered_system_types_; }
        void set_filtered_system_types(std::vector<std::wstring> value) { filtered_system_types_ = std::move(value); }

        void match_result_callback(crt_entry const& crt_entry, uint64_t const user_address, size_units::base_16::bytes const user_size, block_range_match_result const match, heap_node_type const node_type) const
        {
            if(match_result_callback_)
            {
                match_result_callback_(crt_entry, user_address, user_size, match, node_type);
            }
        }
        void set_match_result_callback(std::function<void (crt_entry const& crt_entry, uint64_t user_address, size_units::base_16::bytes user_size, block_range_match_result match, heap_node_type node_type)> value) { match_result_callback_ = std::move(value); }

    private:
        bool no_filter_heap_entries_{false};
        bool no_mark_global_variable_data_pointers_as_system_{false};
        bool no_mark_all_crt_entries_as_system_{false};
        std::vector<std::wstring> filtered_system_types_{};
        segment_heap_front_padding_options_type segment_heap_front_padding_options_{segment_heap_front_padding_options_type::auto_padding_detect};
        std::function<void (crt_entry const& crt_entry, uint64_t user_address, size_units::base_16::bytes user_size, block_range_match_result result, heap_node_type node_type)> match_result_callback_;
    };
}
