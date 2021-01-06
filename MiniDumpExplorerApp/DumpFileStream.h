#pragma once
#include "../DbgHelpUtils/mini_dump.h"
#include "../DbgHelpUtils/symbol_engine.h"

class DbgHelpDispatcher;
struct MiniDumpData;

namespace MiniDumpExplorerApp
{
    struct DumpFileStream : winrt::implements<DumpFileStream, winrt::MiniDumpExplorer::IDumpFileStream>
    {
        DumpFileStream(size_t index, MINIDUMP_DIRECTORY const& directory_entry, dlg_help_utils::mini_dump const& mini_dump, DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] winrt::MiniDumpExplorer::StreamType Type() const;
        [[nodiscard]] winrt::hstring TypeString() const;

        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamComment AsComment() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IDumpFileStreamException AsException() const;

    private:
        size_t index_;
        MINIDUMP_DIRECTORY const& directory_entry_;
        dlg_help_utils::mini_dump const& mini_dump_;
        DbgHelpDispatcher& dispatcher_;
        MiniDumpData& mini_data_data_;
        dlg_help_utils::dbg_help::symbol_engine& symbol_engine_;
    };
}
