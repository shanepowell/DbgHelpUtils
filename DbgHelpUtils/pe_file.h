#pragma once
#include "windows_setup.h"

#include <string>

#include "handles.h"

namespace dlg_help_utils
{
    class pe_file
    {
    public:
        pe_file(std::wstring file_path);
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

    private:
        std::wstring file_path_;
        handles::windows_handle file_;
        handles::map_view_handle map_view_;
        size_t file_length_{0};
        bool is_valid_{false};
        IMAGE_DOS_HEADER const* dos_header_{nullptr};
        bool has_nt_header_{false};
        IMAGE_FILE_HEADER const* nt_header_{nullptr};
        bool is_x86_pe_{false};
        IMAGE_OPTIONAL_HEADER32 const* x86_pe_header_{nullptr};
        bool is_x64_pe_{false};
        IMAGE_OPTIONAL_HEADER64 const* x64_pe_header_{nullptr};
    };
}
