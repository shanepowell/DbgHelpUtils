#pragma once
#include "windows_setup.h"

#include <string>

#include "handles.h"
#include "mini_dump_memory_stream.h"

namespace dlg_help_utils
{
    class pe_file
    {
    public:
        pe_file(std::wstring file_path);
        explicit pe_file(mini_dump_memory_stream stream);
        ~pe_file() = default;

        pe_file(pe_file const&) = delete;
        pe_file(pe_file&&) = default;

        pe_file& operator=(pe_file const&) = delete;
        pe_file& operator=(pe_file&&) = default;

        void open_pe();

        [[nodiscard]] std::wstring const& path() const { return file_path_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t file_length() const { return file_length_; }
        [[nodiscard]] IMAGE_DOS_HEADER const& dos_header() const { return *dos_header_; }
        [[nodiscard]] bool has_nt_header() const { return has_nt_header_; }
        [[nodiscard]] IMAGE_FILE_HEADER const& nt_header() const { return *nt_header_; }

        [[nodiscard]] bool is_x86_header() const
        {
            return (IMAGE_FILE_32BIT_MACHINE & nt_header().Characteristics) == IMAGE_FILE_32BIT_MACHINE;
        }

        [[nodiscard]] bool is_x86_pe() const { return is_x86_pe_; }
        [[nodiscard]] IMAGE_OPTIONAL_HEADER32 const& x86_pe_header() const { return *x86_pe_header_; }
        [[nodiscard]] bool is_x64_pe() const { return is_x64_pe_; }
        [[nodiscard]] IMAGE_OPTIONAL_HEADER64 const& x64_pe_header() const { return *x64_pe_header_; }

        [[nodiscard]] void const* data() const { return dos_header_; }

        [[nodiscard]] std::vector<uint8_t> version_resource_data() const;

    private:
        void open_pe_file();
        void open_pe_stream();
        void load_pe(uint8_t const* start, uint8_t const* end);
        [[nodiscard]] mini_dump_memory_stream find_resource(WORD type, WORD name) const;
        static [[nodiscard]] void const* find_entry_by_id(IMAGE_RESOURCE_DIRECTORY const* dir, WORD id, void const* root, bool want_dir);
        static [[nodiscard]] IMAGE_RESOURCE_DATA_ENTRY const* find_first_entry(IMAGE_RESOURCE_DIRECTORY const* dir, void const* root);

    private:
        std::wstring file_path_;
        mini_dump_memory_stream stream_{};
        handles::windows_handle file_{handles::make_empty_windows_handle()};
        handles::map_view_handle map_view_{handles::make_empty_map_view_handle()};
        size_t file_length_{0};
        bool is_valid_{false};
        IMAGE_DOS_HEADER const* dos_header_{nullptr};
        bool has_nt_header_{false};
        IMAGE_FILE_HEADER const* nt_header_{nullptr};
        bool is_x86_pe_{false};
        IMAGE_OPTIONAL_HEADER32 const* x86_pe_header_{nullptr};
        bool is_x64_pe_{false};
        IMAGE_OPTIONAL_HEADER64 const* x64_pe_header_{nullptr};
        IMAGE_RESOURCE_DIRECTORY const* resource_data_{nullptr};
        size_t resource_data_size_{};
    };
}
