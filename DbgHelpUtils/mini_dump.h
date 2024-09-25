#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <string>

#include "handles.h"

namespace dlg_help_utils
{
    class comment_stream_w;

    enum class dump_file_type : uint8_t
    {
        invalid,
        user_mode_dump,
        kernel_mode_dump_x86,
        kernel_mode_dump_x64,
    };

    class mini_dump
    {
    public:
        explicit mini_dump(std::wstring file_path);

        void open_mini_dump();

        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] dump_file_type type() const { return type_; }
        [[nodiscard]] MINIDUMP_HEADER const* header() const { return header_; }
        [[nodiscard]] MINIDUMP_DIRECTORY const* directory() const { return directory_; }
        [[nodiscard]] void const* data() const { return data_; }
        [[nodiscard]] size_t data_length() const { return data_length_; }
        [[nodiscard]] uint32_t data_crc32() const;
        [[nodiscard]] void const* rva32(MINIDUMP_LOCATION_DESCRIPTOR location) const;
        [[nodiscard]] void const* rva32(RVA rva) const;
        [[nodiscard]] void const* rva64(MINIDUMP_LOCATION_DESCRIPTOR64 location) const;
        [[nodiscard]] void const* rva64(RVA64 rva) const;
        [[nodiscard]] MINIDUMP_DIRECTORY const* find_stream_type(MINIDUMP_STREAM_TYPE type, size_t& index) const;
        [[nodiscard]] MINIDUMP_DIRECTORY const* get_stream_type(MINIDUMP_STREAM_TYPE type, size_t index) const;
        [[nodiscard]] std::wstring const& version_string() const { return version_string_; }

    private:
        void get_kernel_dump_version_string();

    private:
        std::wstring file_path_;
        handles::windows_handle file_;
        handles::map_view_handle map_view_;
        size_t file_length_{0};
        MINIDUMP_HEADER const* header_{nullptr};
        MINIDUMP_DIRECTORY const* directory_{nullptr};
        void const* data_{nullptr};
        size_t data_length_{0};
        dump_file_type type_{dump_file_type::invalid};
        bool is_valid_{false};
        std::wstring version_string_;
    };
}
