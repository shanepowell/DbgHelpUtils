#include "pe_file.h"

#include <format>

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
    {
    }

    pe_file::pe_file(mini_dump_memory_stream stream)
        : stream_{std::move(stream)}
    {
    }

    void pe_file::open_pe()
    {
        if(!file_path_.empty())
        {
            open_pe_file();
        }
        else if(stream_.start_memory_raw() != nullptr)
        {
            open_pe_stream();
        }
    }

    std::vector<uint8_t> pe_file::version_resource_data() const
    {
        std::vector<uint8_t> rv;
        auto stream = find_resource(static_cast<WORD>(reinterpret_cast<uintptr_t>(VS_FILE_INFO)), VS_VERSION_INFO);
        if(stream.eof())
        {
            return rv;
        }

        // ReSharper disable once CppRedundantCastExpression
        rv.resize(static_cast<size_t>(stream.length()));
        [[maybe_unused]] auto read_size = stream.read(rv.data(), rv.size());
        return rv;
    }

    void pe_file::open_pe_file()
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

        if (static_cast<size_t>(file_size.QuadPart) < sizeof(IMAGE_DOS_HEADER))
        {
            throw wide_runtime_error{std::format(L"File: {} to small to be a PE File", file_path_)};
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
        auto const* start = static_cast<uint8_t const*>(map_view_.get());
        auto const* end = start + file_length_;
        load_pe(start, end);
    }

    void pe_file::open_pe_stream()
    {
        auto const* start = static_cast<uint8_t const*>(stream_.start_memory_raw());
        auto const* end = static_cast<uint8_t const*>(stream_.end_memory_raw());
        file_length_ = end - start;
        load_pe(start, end);
    }

    void pe_file::load_pe(uint8_t const* start, uint8_t const* end)
    {
        auto const* current = start;
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
        if (auto const nt_magic = *reinterpret_cast<DWORD const*>(current); nt_magic != IMAGE_NT_SIGNATURE)
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
            switch (*reinterpret_cast<WORD const*>(current))
            {
            case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            {
                if (!is_x86_header() || nt_header_->SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER32) || current +
                    sizeof(IMAGE_OPTIONAL_HEADER32) >= end)
                {
                    return;
                }
                x86_pe_header_ = reinterpret_cast<IMAGE_OPTIONAL_HEADER32 const*>(current);
                if(auto const& resource_directory_data = x86_pe_header_->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
                    resource_directory_data.VirtualAddress > 0 && start + resource_directory_data.VirtualAddress + resource_directory_data.Size <= end)
                {
                    resource_data_ = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY const*>(start + resource_directory_data.VirtualAddress);
                    resource_data_size_ = resource_directory_data.Size;
                }
                is_x86_pe_ = true;
                break;
            }

            case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            {
                if (is_x86_header() || nt_header_->SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER64) || current +
                    sizeof(IMAGE_OPTIONAL_HEADER64) >= end)
                {
                    return;
                }
                x64_pe_header_ = reinterpret_cast<IMAGE_OPTIONAL_HEADER64 const*>(current);
                if(auto const& resource_directory_data = x64_pe_header_->DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
                    resource_directory_data.VirtualAddress > 0 && start + resource_directory_data.VirtualAddress + resource_directory_data.Size <= end)
                {
                    resource_data_ = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY const*>(start + resource_directory_data.VirtualAddress);
                    resource_data_size_ = resource_directory_data.Size;
                }
                is_x64_pe_ = true;
                break;
            }

            default:
                break;
            }
        }
    }

    mini_dump_memory_stream pe_file::find_resource(WORD const type, WORD const name) const
    {
        if(resource_data_ == nullptr || resource_data_size_ == 0)
        {
            return {};
        }

        auto const* dir = static_cast<IMAGE_RESOURCE_DIRECTORY const*>(find_entry_by_id(resource_data_, type, resource_data_, true));
        if(dir == nullptr)
        {
            return {};
        }

        dir = static_cast<IMAGE_RESOURCE_DIRECTORY const*>(find_entry_by_id(dir, name, resource_data_, true));
        if(dir == nullptr)
        {
            return {};
        }

        vector<WORD> search_languages;
        search_languages.emplace_back(static_cast<WORD>(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)));
        search_languages.emplace_back(GetUserDefaultLangID());
        search_languages.emplace_back(GetSystemDefaultLangID());
        search_languages.emplace_back(static_cast<WORD>(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)));

        IMAGE_RESOURCE_DATA_ENTRY const* entry{nullptr};
        for(auto const langId : search_languages)
        {
            entry = static_cast<IMAGE_RESOURCE_DATA_ENTRY const*>(find_entry_by_id(dir, langId, resource_data_, false));
            if(entry != nullptr)
            {
                break;
            }
        }

        if(entry == nullptr)
        {
            entry = find_first_entry(dir, resource_data_);
        }

        return {static_cast<uint8_t const*>(data()) + entry->OffsetToData, entry->Size};
    }

    void const* pe_file::find_entry_by_id(IMAGE_RESOURCE_DIRECTORY const* dir, WORD const id, void const* root, bool const want_dir)
    {
        auto const* entry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY const*>(dir + 1);
        auto min = dir->NumberOfNamedEntries;
        auto max = min + dir->NumberOfIdEntries - 1;

        while (min <= max)
        {
            auto const pos = static_cast<WORD>(min + max) / 2;
            if (entry[pos].Id == id)
            {
                if (!entry[pos].DataIsDirectory == !want_dir)
                {
                    return static_cast<uint8_t const*>(root) + entry[pos].OffsetToDirectory;
                }
                break;
            }

            if (entry[pos].Id > id) 
            {
                max = pos - 1;
            }
            else
            {
                min = static_cast<WORD>(pos + 1);
            }
        }
        return nullptr;
    }

    IMAGE_RESOURCE_DATA_ENTRY const* pe_file::find_first_entry(IMAGE_RESOURCE_DIRECTORY const* dir, void const* root)
    {
        auto const* entry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY const *>(dir + 1);
        for (int pos = 0; pos < dir->NumberOfNamedEntries + dir->NumberOfIdEntries; pos++)
        {
            if (!entry[pos].DataIsDirectory == true)
            {
                return reinterpret_cast<IMAGE_RESOURCE_DATA_ENTRY const*>(static_cast<uint8_t const*>(root) + entry[pos].OffsetToDirectory);
            }
        }
        return nullptr;
    }
}
