#include "pch.h"
#include "StackTrace.h"

#include "DbgHelpDispatcher.h"
#include "MiniDumpData.h"
#include "StackFrame.h"
#include "../DbgHelpUtils/mini_dump_stack_walk.h"
#include "../DbgHelpUtils/stream_thread_context.h"
#include "../DbgHelpUtils/symbol_engine.h"


namespace MiniDumpExplorerApp
{

StackTrace::StackTrace(DbgHelpDispatcher& dispatcher, MiniDumpData& mini_data_data, dlg_help_utils::dbg_help::symbol_engine& symbol_engine, uint64_t const stack_start_address, void const* stack, size_t const stack_size, dlg_help_utils::stream_thread_context const& thread_context)
    : thread_context_{thread_context}
{
    dispatcher.Post([this, anchor = get_strong(), &mini_data_data, &symbol_engine, stack_start_address, stack, stack_size]
    {
        dlg_help_utils::stream_stack_dump::mini_dump_stack_walk walk_callback{
            stack_start_address, stack, stack_size, mini_data_data.memory_list_, mini_data_data.memory64_list_, mini_data_data.function_table_, mini_data_data.module_list_,
            mini_data_data.unloaded_module_list_, mini_data_data.pe_file_memory_mappings_, symbol_engine
        };

        auto const is_x86_address = Is32BitAddresses();

        for (auto const& entry : dlg_help_utils::dbg_help::symbol_engine::stack_walk(thread_context_, walk_callback))
        {
            frames_.Append(*winrt::make_self<StackFrame>(entry, is_x86_address));
        }
    });
}

bool StackTrace::Is32BitAddresses() const
{
    return thread_context_.x86_thread_context_available() || thread_context_.wow64_thread_context_available();
}

winrt::Windows::Foundation::Collections::IObservableVector<winrt::MiniDumpExplorer::IStackFrame> StackTrace::Frames() const
{
    return frames_;
}

}
