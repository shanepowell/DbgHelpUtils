#pragma once
#include "../DbgHelpUtils/mini_dump.h"

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace MiniDumpExplorerApp
{
    struct DumpFile : winrt::implements<DumpFile, winrt::MiniDumpExplorer::IDumpFile>
    {
        DumpFile(winrt::hstring path, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] winrt::MiniDumpExplorer::DumpFileType Type() const { return type_; }
        [[nodiscard]] bool HasHeader() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileHeader Header();
        [[nodiscard]] winrt::Windows::Foundation::Collections::IVector<winrt::MiniDumpExplorer::IDumpFileStream> Streams() const;

    private:
        void LoadStreams() const;

    private:
        dlg_help_utils::mini_dump dump_file_;
        dlg_help_utils::dbg_help::symbol_engine& symbol_engine_;
        winrt::MiniDumpExplorer::DumpFileType type_{winrt::MiniDumpExplorer::DumpFileType::Invalid};
        winrt::Windows::Foundation::Collections::IVector<winrt::MiniDumpExplorer::IDumpFileStream> streams_{winrt::single_threaded_vector<winrt::MiniDumpExplorer::IDumpFileStream>()};
    };
}
