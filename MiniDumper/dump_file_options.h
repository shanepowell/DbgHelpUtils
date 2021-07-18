#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>
#include <vector>
#include <boost/program_options.hpp>

class dump_file_options
{
public:
    explicit dump_file_options(boost::program_options::variables_map const& vm);

    [[nodiscard]] bool dump_header() const { return dump_header_; }
    [[nodiscard]] bool dump_streams() const { return dump_streams_; }
    [[nodiscard]] bool generate_crc32() const { return generate_crc32_; }
    [[nodiscard]] bool hex_dump_stream_data() const { return hex_dump_stream_data_; }
    [[nodiscard]] bool hex_dump_memory_data() const { return hex_dump_memory_data_; }
    [[nodiscard]] std::vector<size_t> const& dump_stream_indexes() const { return dump_stream_indexes_; }
    [[nodiscard]] std::vector<MINIDUMP_STREAM_TYPE> const& dump_stream_types() const { return dump_stream_types_; }
    [[nodiscard]] bool continue_on_errors() const { return continue_on_errors_; }
    [[nodiscard]] bool load_symbols() const { return load_symbols_; }
    [[nodiscard]] bool debug_symbols() const { return debug_symbols_; }
    [[nodiscard]] bool debug_load_symbols_memory() const { return debug_load_symbols_memory_; }
    [[nodiscard]] bool debug_type_data() const { return debug_type_data_; }
    [[nodiscard]] bool display_peb() const { return display_peb_; }
    [[nodiscard]] std::vector<std::wstring> const& filter_values(std::wstring const& option) const;
    [[nodiscard]] std::vector<std::wstring> symbol_types() const { return symbol_types_; }
    [[nodiscard]] std::vector<std::wstring> dump_types_modules() const { return dump_types_modules_; }
    [[nodiscard]] std::vector<std::wstring> dump_address_types() const { return dump_address_types_; }

private:
    bool dump_header_;
    bool dump_streams_;
    bool generate_crc32_;
    bool hex_dump_stream_data_;
    bool hex_dump_memory_data_;
    std::vector<size_t> dump_stream_indexes_;
    std::vector<MINIDUMP_STREAM_TYPE> dump_stream_types_;
    bool continue_on_errors_;
    bool load_symbols_;
    bool debug_symbols_;
    bool debug_load_symbols_memory_;
    bool debug_type_data_;
    bool display_peb_;
    std::unordered_map<std::wstring, std::vector<std::wstring>> filter_values_;
    std::vector<std::wstring> symbol_types_;
    std::vector<std::wstring> dump_types_modules_;
    std::vector<std::wstring> dump_address_types_;
};
