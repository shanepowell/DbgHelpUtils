#pragma once
#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileHeader : winrt::implements<DumpFileHeader, winrt::MiniDumpExplorer::IDumpFileHeader>
    {
        DumpFileHeader(dlg_help_utils::mini_dump& dump_file, winrt::MiniDumpExplorer::DumpFileType type);

        [[nodiscard]] winrt::MiniDumpExplorer::DumpFileType Type() const;
        [[nodiscard]] winrt::hstring TypeString() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] winrt::hstring VersionString() const;
        [[nodiscard]] uint32_t Signature() const;
        [[nodiscard]] uint16_t Version() const;
        [[nodiscard]] uint16_t InternalVersion() const;
        [[nodiscard]] uint32_t NumberOfStreams() const;
        [[nodiscard]] uint32_t StreamDirectoryRva() const;
        [[nodiscard]] uint32_t CheckSum() const;
        [[nodiscard]] winrt::clock::time_point TimeDateStamp() const;
        [[nodiscard]] uint64_t Flags() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FlagTypes() const;

    private:
        dlg_help_utils::mini_dump& dump_file_;
        winrt::MiniDumpExplorer::DumpFileType type_;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> flag_types_{winrt::single_threaded_observable_vector<winrt::hstring>()};
    };
}
