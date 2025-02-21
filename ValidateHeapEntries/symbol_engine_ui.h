#pragma once
#include "DbgHelpUtils/i_symbol_load_callback.h"
#include "DbgHelpUtils/null_stream.h"

class symbol_engine_ui : public dlg_help_utils::dbg_help::i_symbol_load_callback
{
public:
    symbol_engine_ui(bool no_output);

    [[nodiscard]] bool deferred_symbol_load_cancel(std::wstring_view const& module_name) override;
    void deferred_symbol_load_partial(std::wstring_view const& module_name) override;
    void start_download(std::wstring_view const& module_name) override;
    void download_percent(unsigned percent) override;
    void download_complete() override;
    [[nodiscard]] dlg_help_utils::dbg_help::log_handle log_stream() const override;
    [[nodiscard]] bool symbol_load_debug() const override;
    [[nodiscard]] bool symbol_load_debug_memory() const override;

private:
    bool no_output_;
    std::wstring module_;
    std::wstring last_percent_;
    mutable null_stream null_stream_;
};
