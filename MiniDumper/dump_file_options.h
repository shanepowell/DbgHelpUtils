#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4100 4458)
#include <lyra/lyra.hpp>
#pragma warning(pop)


#include <fstream>

#include "DbgHelpUtils/process_heaps_options.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/statistic_view_options.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/system_module_list.h"

namespace heap_statistics_view
{
    constexpr uint16_t by_size_frequency_view                   = 0x01;
    constexpr uint16_t by_size_ranges_frequency_view            = 0x02;
    constexpr uint16_t by_stacktrace_frequency_view             = 0x04;
    constexpr uint16_t by_application_callsite_frequency_view   = 0x08;
    constexpr uint16_t all_views                                = 0x0F;
}

class dump_file_options
{
public:

    dump_file_options() = default;

    lyra::cli generate_options();

    void process_raw_options();

    [[nodiscard]] bool display_version() const { return display_version_; }
    [[nodiscard]] bool dump_header() const { return dump_header_; }
    [[nodiscard]] bool dump_streams() const { return dump_streams_; }
    [[nodiscard]] bool generate_crc32() const { return generate_crc32_; }
    [[nodiscard]] bool hex_dump_stream_data() const { return hex_dump_stream_data_; }
    [[nodiscard]] bool hex_dump_memory_data() const { return hex_dump_memory_data_; }
    [[nodiscard]] uint64_t limit_hex_dump_memory_size() const { return limit_hex_dump_memory_size_; }
    [[nodiscard]] uint64_t hex_dump_memory_size(uint64_t size) const;
    [[nodiscard]] std::vector<size_t> const& dump_stream_indexes() const { return dump_stream_indexes_; }
    [[nodiscard]] std::vector<MINIDUMP_STREAM_TYPE> const& dump_stream_types() const { return dump_stream_types_; }
    [[nodiscard]] bool dump_all_stream_indexes() const { return dump_all_stream_indexes_; }
    [[nodiscard]] bool continue_on_errors() const { return continue_on_errors_; }
    [[nodiscard]] bool display_symbols() const { return display_symbols_; }
    [[nodiscard]] bool debug_symbols() const { return debug_symbols_ || debug_load_symbols_memory_; }
    [[nodiscard]] bool debug_load_symbols_memory() const { return debug_load_symbols_memory_; }
    [[nodiscard]] bool debug_type_data() const { return debug_type_data_; }
    [[nodiscard]] bool debug_type_parent_data() const { return debug_type_parent_data_; }
    [[nodiscard]] bool display_peb() const { return display_peb_; }
    [[nodiscard]] bool display_heap() const { return display_heap_; }
    [[nodiscard]] bool display_heap_entries() const { return display_heap_entries_; }
    [[nodiscard]] bool display_heap_statistic_views() const { return heap_statistics_views_ != 0x0; }
    [[nodiscard]] bool display_heap_statistic_view(uint16_t const view) const { return (heap_statistics_views_ & view) == view; }
    [[nodiscard]] bool display_crtheap() const { return display_crtheap_; }
    [[nodiscard]] bool debug_heap_data() const { return debug_heap_data_; }
    [[nodiscard]] bool display_stack_trace_database() const { return display_stack_trace_database_; }
    [[nodiscard]] bool disable_symbol_load_cancel_keyboard_check() const { return disable_symbol_load_cancel_keyboard_check_; }
    [[nodiscard]] bool display_heap_graph() const { return display_heap_graph_; }
    [[nodiscard]] bool display_loaded_modules() const { return display_loaded_modules_; }
    [[nodiscard]] size_t display_heap_graph_to_reference_limit() const { return display_heap_graph_to_reference_limit_; }
    [[nodiscard]] dlg_help_utils::stream_stack_dump::dump_stack_options display_stack_options() const;
    [[nodiscard]] std::vector<std::wstring> const& filter_values(std::wstring const& option) const;
    [[nodiscard]] std::vector<std::wstring> const& symbol_types() const { return symbol_types_; }
    [[nodiscard]] std::vector<std::wstring> const& symbol_names() const { return symbol_names_; }
    [[nodiscard]] bool list_symbol_names() const { return list_symbol_names_; }
    [[nodiscard]] std::vector<std::wstring> const& dump_types_modules() const { return dump_types_modules_; }
    [[nodiscard]] std::vector<std::wstring> const& dump_address_types() const { return dump_address_types_; }
    [[nodiscard]] std::vector<std::wstring> const& dump_files() const { return dump_files_; }
    [[nodiscard]] std::vector<std::wstring> const& base_diff_dump_files() const { return base_diff_dump_files_; }
    [[nodiscard]] dlg_help_utils::heap::system_module_list const& system_module_list() const { return system_module_list_; }
    [[nodiscard]] dlg_help_utils::heap::statistic_views::statistic_view_options const& statistic_view_options() const { return statistic_view_options_; }
    [[nodiscard]] dlg_help_utils::heap::process_heaps_options const& process_heaps_options() const { return process_heaps_options_; }
    [[nodiscard]] dlg_help_utils::heap::process_heaps_options& process_heaps_options() { return process_heaps_options_; }
    [[nodiscard]] bool verbose_output() const { return verbose_output_; }
    [[nodiscard]] size_t graph_display_max_call_depth() const { return graph_display_max_call_depth_; }
    [[nodiscard]] std::wostream& get_log_stream();

private:
    static std::vector<std::wstring> convert_to_wstring(std::vector<std::string>& raw);

private:
    bool display_version_{false};
    bool dump_header_{false};
    bool dump_streams_{false};
    bool generate_crc32_{false};
    bool hex_dump_stream_data_{false};
    bool hex_dump_memory_data_{false};
    std::string limit_hex_dump_memory_size_raw_;
    uint64_t limit_hex_dump_memory_size_{0};
    std::vector<size_t> dump_stream_indexes_;
    std::vector<std::string> dump_stream_types_raw_;
    std::vector<MINIDUMP_STREAM_TYPE> dump_stream_types_;
    bool continue_on_errors_{false};
    bool dump_all_stream_indexes_{false};
    bool display_symbols_{false};
    bool debug_symbols_{false};
    bool debug_load_symbols_memory_{false};
    bool debug_type_data_{false};
    bool debug_type_parent_data_{false};
    bool display_peb_{false};
    bool display_heap_{false};
    bool display_heap_entries_{false};
    bool display_crtheap_{false};
    bool debug_heap_data_{false};
    bool display_stack_trace_database_{false};
    bool disable_symbol_load_cancel_keyboard_check_{false};
    bool display_stack_variables_{false};
    bool display_stack_parameters_{false};
    bool display_heap_graph_{false};
    bool display_loaded_modules_{false};
    std::string display_heap_graph_to_reference_limit_raw_;
    size_t display_heap_graph_to_reference_limit_{10};
    uint16_t heap_statistics_views_{0};
    std::vector<std::string> filter_values_raw_;
    std::unordered_map<std::wstring, std::vector<std::wstring>> filter_values_;
    std::vector<std::string> dump_files_raw_;
    std::vector<std::wstring> dump_files_;
    std::vector<std::string> base_diff_dump_files_raw_;
    std::vector<std::wstring> base_diff_dump_files_;
    std::vector<std::string> symbol_names_raw_;
    std::vector<std::wstring> symbol_names_;
    bool list_symbol_names_{false};
    std::vector<std::string> symbol_types_raw_;
    std::vector<std::wstring> symbol_types_;
    std::vector<std::string> dump_types_modules_raw_;
    std::vector<std::wstring> dump_types_modules_;
    std::vector<std::string> dump_address_types_raw_;
    std::vector<std::wstring> dump_address_types_;
    std::vector<std::string> heap_statistics_raw_;
    std::vector<std::string> filtered_system_types_raw_;
    std::string by_range_view_range_raw_;
    std::string system_module_list_file_;
    std::string view_sort_column_raw_;
    std::string view_sort_order_raw_;
    std::string output_filename_raw_;
    std::wstring output_filename_;
    std::string segment_heap_front_padding_options_raw_;
    dlg_help_utils::heap::system_module_list system_module_list_;
    dlg_help_utils::heap::statistic_views::statistic_view_options statistic_view_options_;
    dlg_help_utils::heap::process_heaps_options process_heaps_options_;
    std::wofstream log_file_;
    bool verbose_output_{false};
    std::string graph_display_max_call_depth_raw_;
    size_t graph_display_max_call_depth_{50};
};
