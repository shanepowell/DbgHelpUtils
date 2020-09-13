#include "pe_file.h"

#include <sstream>

#include "windows_error.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;
using namespace std;
using namespace dlg_help_utils::handles;
using namespace dlg_help_utils::windows_error;
using namespace dlg_help_utils::exceptions;

namespace dlg_help_utils
{
    pe_file::pe_file(std::wstring file_path)
        : file_path_{std::move(file_path)}
          , file_{make_empty_windows_handle()}
          , map_view_{make_empty_map_view_handle()}
    {
    }

    void pe_file::open_pe()
    {
        file_ = make_windows_handle(CreateFile(file_path_.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                               OPEN_EXISTING, 0, nullptr));
        if (!file_)
        {
            auto const ec = GetLastError();
            throw wide_runtime_error{
                (wostringstream{} << L"Error: CreateFile failed. Error: " << ec << L" - " <<
                    get_windows_error_string(ec)).str()
            };
        }

        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(file_.get(), &file_size))
        {
            auto const ec = GetLastError();
            throw wide_runtime_error{
                (wostringstream{} << L"Error: GetFileSizeEx failed. Error: " << ec << L" - " <<
                    get_windows_error_string(ec)).str()
            };
        }

        if (static_cast<size_t>(file_size.QuadPart) < sizeof(IMAGE_DOS_HEADER))
        {
            throw wide_runtime_error{
                (wostringstream{} << L"File: " << file_path_ << L" to small to be a PE File").str()
            };
        }

        file_length_ = static_cast<size_t>(file_size.QuadPart);

        auto const map_file =
            make_windows_handle(CreateFileMapping(file_.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
        if (!map_file)
        {
            auto const ec = GetLastError();
            throw wide_runtime_error{
                (wostringstream{} << L"Error: CreateFileMapping failed. Error: " << ec << L" - " <<
                    get_windows_error_string(ec)).str()
            };
        }

        map_view_ = make_map_view_handle(MapViewOfFile(map_file.get(), FILE_MAP_READ, 0, 0, 0));
        if (map_view_ == nullptr)
        {
            auto const ec = GetLastError();
            throw wide_runtime_error{
                (wostringstream{} << L"Error: MapViewOfFile failed. Error: " << ec << L" - " <<
                    get_windows_error_string(ec)).str()
            };
        }

        auto const* current = static_cast<uint8_t const*>(map_view_.get());
        auto const* end = current + file_length_;
        dos_header_ = reinterpret_cast<IMAGE_DOS_HEADER const*>(current);
        if (dos_header_->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return;
        }

        is_valid_ = true;

        if (dos_header_->e_lfanew == 0 || static_cast<size_t>(dos_header_->e_lfanew) + sizeof(WORD) > file_length_)
        {
            return;
        }

        current += dos_header_->e_lfanew;
        auto const nt_magic = *reinterpret_cast<DWORD const*>(current);
        if (nt_magic != IMAGE_NT_SIGNATURE)
        {
            return;
        }
        current += sizeof(DWORD);
        if (current + sizeof(IMAGE_FILE_HEADER) >= end)
        {
            return;
        }
        has_nt_header_ = true;
        nt_header_ = reinterpret_cast<IMAGE_FILE_HEADER const*>(current);

        current += sizeof(IMAGE_FILE_HEADER);

        if (nt_header_->SizeOfOptionalHeader > 0)
        {
            auto const magic = *reinterpret_cast<WORD const*>(current);
            switch (magic)
            {
            case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
                if (!is_x86_header() || nt_header_->SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER32) || current +
                    sizeof(IMAGE_OPTIONAL_HEADER32) >= end)
                {
                    return;
                }
                x86_pe_header_ = reinterpret_cast<IMAGE_OPTIONAL_HEADER32 const*>(current);
                is_x86_pe_ = true;
                break;

            case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
                if (is_x86_header() || nt_header_->SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER64) || current +
                    sizeof(IMAGE_OPTIONAL_HEADER64) >= end)
                {
                    return;
                }
                x64_pe_header_ = reinterpret_cast<IMAGE_OPTIONAL_HEADER64 const*>(current);
                is_x64_pe_ = true;
                break;

            default:
                break;
            }
        }
    }
}
