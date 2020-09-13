#pragma once

#include "dump_file_options.h"
#include "DbgHelpUtils/i_symbol_load_callback.h"

class symbol_engine_ui : public dlg_help_utils::dbg_help::i_symbol_load_callback
{
public:
    explicit symbol_engine_ui(dump_file_options const& options);

    [[nodiscard]] bool deferred_symbol_load_cancel(std::wstring_view const& module) override;
    void deferred_symbol_load_partial(std::wstring_view const& module) override;
    void start_download(std::wstring_view const& module) override;
    void download_percent(unsigned percent) override;
    void download_complete() override;
    [[nodiscard]] std::wostream& log_stream() const override;
    [[nodiscard]] bool symbol_load_debug() const override;
    [[nodiscard]] bool symbol_load_debug_memory() const override;

private:
    dump_file_options const& options_;
    std::wstring module_;
    std::wstring last_percent_;
};
