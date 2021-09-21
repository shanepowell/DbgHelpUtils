#include "mini_dump.h"

#include <format>

#include "crc.h"
#include "stream_hex_dump.h"
#include "windows_error.h"
#include "wide_runtime_error.h"

using namespace std::string_literals;
using namespace std;
using namespace dlg_help_utils::handles;
using namespace dlg_help_utils::windows_error;
using namespace dlg_help_utils::exceptions;
using namespace dlg_help_utils::crc;

#define KERNEL_X86_DUMP_SIGNATURE	('EGAP')	// NOLINT(cppcoreguidelines-macro-usage)
#define KERNEL_X86_VERSION			('PMUD')	// NOLINT(cppcoreguidelines-macro-usage)

#define KERNEL_X64_DUMP_SIGNATURE	('EGAP')	// NOLINT(cppcoreguidelines-macro-usage)
#define KERNEL_X64_VERSION			('46UD')	// NOLINT(cppcoreguidelines-macro-usage)


namespace dlg_help_utils
{
    mini_dump::mini_dump(wstring file_path)
    : file_path_{move(file_path)}
    , file_{make_empty_windows_handle()}
    , map_view_{make_empty_map_view_handle()}
    {
    }

    void mini_dump::open_mini_dump()
    {
        file_ = make_windows_handle(CreateFile(file_path_.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                               OPEN_EXISTING, 0, nullptr));
        if (!file_)
        {
            throw_windows_api_error(L"CreateFile"sv, file_path_);
        }

        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(file_.get(), &file_size))
        {
            throw_windows_api_error(L"GetFileSizeEx"sv, file_path_);
        }

        if (static_cast<size_t>(file_size.QuadPart) < sizeof(MINIDUMP_HEADER))
        {
            throw wide_runtime_error{std::format(L"File: {} to small to be a MiniDumpFile", file_path_)};
        }

        file_length_ = static_cast<size_t>(file_size.QuadPart);

        auto const map_file =
            make_windows_handle(CreateFileMapping(file_.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
        if (!map_file)
        {
            throw_windows_api_error(L"CreateFileMapping"sv, file_path_);
        }

        map_view_ = make_map_view_handle(MapViewOfFile(map_file.get(), FILE_MAP_READ, 0, 0, 0));
        if (map_view_ == nullptr)
        {
            throw_windows_api_error(L"MapViewOfFile"sv, file_path_);
        }

        header_ = static_cast<MINIDUMP_HEADER const*>(map_view_.get());
        data_ = static_cast<uint8_t const*>(map_view_.get()) + sizeof(MINIDUMP_HEADER);
        data_length_ = file_length_ - sizeof(MINIDUMP_HEADER);

        // valid dump files:
        if (header_->Signature == MINIDUMP_SIGNATURE && LOWORD(header_->Version) == MINIDUMP_VERSION)
        {
            // user mode dump file
            type_ = dump_file_type::user_mode_dump;
            version_string_ = string_conversation::acp_to_wstring(string_view{
                reinterpret_cast<char const*>(&header_->Signature), sizeof(header_->Signature)
            });
        }
        else if (header_->Signature == KERNEL_X86_DUMP_SIGNATURE && header_->Version == KERNEL_X86_VERSION)
        {
            // kernel mode x86 dump file
            type_ = dump_file_type::kernel_mode_dump_x86;
            get_kernel_dump_version_string();
        }
        else if (header_->Signature == KERNEL_X64_DUMP_SIGNATURE && header_->Version == KERNEL_X64_VERSION)
        {
            // kernel mode x64 dump file
            type_ = dump_file_type::kernel_mode_dump_x64;
            get_kernel_dump_version_string();
        }
        else
        {
            throw wide_runtime_error{std::format(L"File: {} has invalid MiniDump signature", file_path_)};
        }

        if (header_->StreamDirectoryRva < sizeof(MINIDUMP_HEADER) || header_->StreamDirectoryRva > file_length_ - (
            sizeof(MINIDUMP_DIRECTORY) * header_->NumberOfStreams))
        {
            throw wide_runtime_error{std::format(L"File: {} has invalid StreamDirectoryRva offset", file_path_)};
        }
        directory_ = reinterpret_cast<MINIDUMP_DIRECTORY const*>(static_cast<uint8_t const*>(map_view_.get()) + header_
            ->StreamDirectoryRva);
        is_valid_ = true;
    }

    uint32_t mini_dump::data_crc32() const
    {
        return crc32(data(), data_length());
    }

    void const* mini_dump::rva32(MINIDUMP_LOCATION_DESCRIPTOR const location) const
    {
        if (location.Rva < sizeof(MINIDUMP_HEADER) || location.Rva > file_length_ - location.DataSize)
        {
            throw wide_runtime_error{std::format(L"File: {0} has invalid invalid RVA32[{1} - {2}] offset", file_path_, location.Rva, location.DataSize)};
        }

        return static_cast<uint8_t const*>(data()) + (location.Rva - sizeof(MINIDUMP_HEADER));
    }

    void const* mini_dump::rva32(RVA const rva) const
    {
        if (rva < sizeof(MINIDUMP_HEADER) || rva >= file_length_)
        {
            throw wide_runtime_error{std::format(L"File: {0} has invalid invalid RVA32[{1}] offset", file_path_, rva)};
        }

        return static_cast<uint8_t const*>(data()) + (rva - sizeof(MINIDUMP_HEADER));
    }

    void const* mini_dump::rva64(MINIDUMP_LOCATION_DESCRIPTOR64 const location) const
    {
        if (location.Rva < sizeof(MINIDUMP_HEADER) || location.Rva > file_length_ - location.DataSize)
        {
            throw wide_runtime_error{std::format(L"File: {0} has invalid invalid RVA64[{1}] offset", file_path_, location.Rva)};
        }

        return static_cast<uint8_t const*>(data()) + (location.Rva - sizeof(MINIDUMP_HEADER));
    }

    void const* mini_dump::rva64(RVA64 const rva) const
    {
        if (rva < sizeof(MINIDUMP_HEADER) || rva >= file_length_)
        {
            throw wide_runtime_error{std::format(L"File: {0} has invalid invalid RVA64[{1}] offset", file_path_, rva)};
        }

        return static_cast<uint8_t const*>(data()) + (rva - sizeof(MINIDUMP_HEADER));
    }

    MINIDUMP_DIRECTORY const* mini_dump::find_stream_type(MINIDUMP_STREAM_TYPE const type, size_t& index) const
    {
        if (header_ == nullptr)
        {
            throw wide_runtime_error{L"no header found"s};
        }

        if (directory_ == nullptr)
        {
            throw wide_runtime_error{L"no stream directory found"s};
        }

        for (; index < header_->NumberOfStreams; ++index)
        {
            auto const& entry = directory_[index];
            if (static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType) == type)
            {
                return &entry;
            }
        }

        return nullptr;
    }

    void mini_dump::get_kernel_dump_version_string()
    {
        version_string_ = string_conversation::acp_to_wstring(string_view{
            reinterpret_cast<char const*>(&header_->Signature), sizeof(header_->Signature) + sizeof(header_->Version)
        });
    }
}
