#pragma once

struct MiniDumpData;
class DbgHelpDispatcher;

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_engine;
    }

    class stream_thread_context;
}

namespace MiniDumpExplorerApp
{
    struct StackTrace : winrt::implements<StackTrace, winrt::MiniDumpExplorer::IStackTrace>
    {
        StackTrace(DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, uint64_t stack_start_address, void const* stack, size_t stack_size, dlg_help_utils::stream_thread_context const& thread_context);

        [[nodiscard]]  bool Is32BitAddresses() const;
        [[nodiscard]] winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::IStackFrame> Frames() const;

    private:
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::IStackFrame> frames_{winrt::multi_threaded_observable_vector<winrt::MiniDumpExplorer::IStackFrame>()};
        dlg_help_utils::stream_thread_context const& thread_context_;
    };
}
