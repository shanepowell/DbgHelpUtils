#pragma once

#include <winrt/Windows.System.h>

#include "DbgHelpUtils/i_symbol_load_callback.h"
#include "DbgHelpUtils/null_stream.h"
#include "DbgHelpUtils/symbol_engine.h"

class SymbolEngineHelper : dlg_help_utils::dbg_help::i_symbol_load_callback
{
private:
    struct Key {};

public:
    SymbolEngineHelper(Key);
    static SymbolEngineHelper& Instance();

    winrt::Windows::System::DispatcherQueueController& QueueController()
    {
        return dbg_help_queue_;
    }

    dlg_help_utils::dbg_help::symbol_engine& symbol_engine()
    {
        return symbol_engine_;
    }

private:
    [[nodiscard]] bool deferred_symbol_load_cancel(std::wstring_view const& module_name) override;
    void deferred_symbol_load_partial(std::wstring_view const& module_name) override;
    void start_download(std::wstring_view const& module_name) override;
    void download_percent(unsigned percent) override;
    void download_complete() override;
    [[nodiscard]] dlg_help_utils::dbg_help::log_handle log_stream() const override;
    [[nodiscard]] bool symbol_load_debug() const override;
    [[nodiscard]] bool symbol_load_debug_memory() const override;
    void log_complete() const;

private:
    winrt::Windows::System::DispatcherQueueController dbg_help_queue_;
    dlg_help_utils::dbg_help::symbol_engine symbol_engine_;
    mutable std::wostringstream log_;
};
