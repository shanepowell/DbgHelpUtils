#include "pch.h"
#include "DumpFileHeader.h"

#include "../DbgHelpUtils/mini_dump_type.h"
#include "../DbgHelpUtils/system_info_utils.h"

namespace MiniDumpExplorerApp
{
    DumpFileHeader::DumpFileHeader(dlg_help_utils::mini_dump& dump_file, winrt::MiniDumpExplorer::DumpFileType type)
        : dump_file_{dump_file}
        , type_{type}
    {
        auto const* header = dump_file.header();
        if (header == nullptr)
        {
            return;
        }

        for (auto const& flag_type : dlg_help_utils::mini_dump_type::to_strings(static_cast<MINIDUMP_TYPE>(header->Flags)))
        {
            flag_types_.Append(winrt::hstring{flag_type});
        }
    }

    winrt::MiniDumpExplorer::DumpFileType DumpFileHeader::Type() const
    {
        return type_;
    }

    winrt::hstring DumpFileHeader::TypeString() const
    {
        return winrt::hstring{dlg_help_utils::system_info_utils::mini_dump_type_to_string(dump_file_.type())};
    }

    bool DumpFileHeader::IsValid() const
    {
        return dump_file_.is_valid();
    }

    winrt::hstring DumpFileHeader::VersionString() const
    {
        return winrt::hstring{dump_file_.version_string()};
    }

    uint32_t DumpFileHeader::Signature() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return header->Signature;
    }

    uint16_t DumpFileHeader::Version() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return LOWORD(header->Version);
    }

    uint16_t DumpFileHeader::InternalVersion() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return HIWORD(header->Version);
    }

    uint32_t DumpFileHeader::NumberOfStreams() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return header->NumberOfStreams;
    }

    uint32_t DumpFileHeader::StreamDirectoryRva() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return header->StreamDirectoryRva;
    }

    uint32_t DumpFileHeader::CheckSum() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return header->CheckSum;
    }

    winrt::clock::time_point DumpFileHeader::TimeDateStamp() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return {};
        }

        return winrt::clock::from_time_t(header->TimeDateStamp);
    }

    uint64_t DumpFileHeader::Flags() const
    {
        auto const* header = dump_file_.header();
        if (header == nullptr)
        {
            return 0;
        }

        return header->Flags;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> DumpFileHeader::FlagTypes() const
    {
        return flag_types_;
    }
}
