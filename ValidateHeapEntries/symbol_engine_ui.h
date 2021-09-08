﻿#pragma once
#include "DbgHelpUtils/i_symbol_load_callback.h"

class symbol_engine_ui : public dlg_help_utils::dbg_help::i_symbol_load_callback
{
public:
    symbol_engine_ui() = default;

    [[nodiscard]] bool deferred_symbol_load_cancel(std::wstring_view const& module_name) override;
    void deferred_symbol_load_partial(std::wstring_view const& module_name) override;
    void start_download(std::wstring_view const& module_name) override;
    void download_percent(unsigned percent) override;
    void download_complete() override;
    [[nodiscard]] std::wostream& log_stream() const override;
    [[nodiscard]] bool symbol_load_debug() const override;
    [[nodiscard]] bool symbol_load_debug_memory() const override;

private:
    std::wstring module_;
    std::wstring last_percent_;
};
