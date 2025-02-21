#include "symbol_engine_ui.h"

#include <format>
#include <iostream>

symbol_engine_ui::symbol_engine_ui(dump_file_options const& options)
    : options_{options}
{
}

bool symbol_engine_ui::deferred_symbol_load_cancel([[maybe_unused]] std::wstring_view const& module_name)
{
    if (!options_.disable_symbol_load_cancel_keyboard_check() && ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) == 0x8000 || (GetAsyncKeyState(VK_RCONTROL) & 0x8000) == 0x8000))
    {
        return true;
    }
    return false;
}

void symbol_engine_ui::deferred_symbol_load_partial([[maybe_unused]] std::wstring_view const& module_name)
{
}

void symbol_engine_ui::start_download(std::wstring_view const& module_name)
{
    module_ = std::format(L"downloading {}: ", module_name);
    if(options_.verbose_output())
    {
        std::wcerr << module_;
        last_percent_.clear();
    }
}

void symbol_engine_ui::download_percent(unsigned const percent)
{
    if (options_.verbose_output() && !last_percent_.empty())
    {
        std::wcerr << std::wstring(last_percent_.size(), L'\b');
    }

    last_percent_ = std::format(L"{}%", percent);
    if(options_.verbose_output())
    {
        std::wcerr << last_percent_;
    }
}

void symbol_engine_ui::download_complete()
{
    if (options_.verbose_output() && (!last_percent_.empty() || !module_.empty()))
    {
        std::wstring const clear(last_percent_.size() + module_.size(), L'\b');
        std::wcerr << clear << std::wstring(last_percent_.size() + module_.size(), L' ') << clear;
    }
    last_percent_.clear();
    module_.clear();
}

dlg_help_utils::dbg_help::log_handle symbol_engine_ui::log_stream() const
{
    if(options_.verbose_output())
    {
        return {std::wcerr, {}};
    }

    return {null_stream_, {}};
}

bool symbol_engine_ui::symbol_load_debug() const
{
    return options_.debug_symbols();
}

bool symbol_engine_ui::symbol_load_debug_memory() const
{
    return options_.debug_load_symbols_memory();
}
