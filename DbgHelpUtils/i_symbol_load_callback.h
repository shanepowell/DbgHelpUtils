#pragma once

// ReSharper disable CppUnusedIncludeDirective
#include <string_view>
#include <ostream>
// ReSharper restore CppUnusedIncludeDirective

namespace dlg_help_utils::dbg_help
{
    class i_symbol_load_callback
    {
    public:
        i_symbol_load_callback() = default;
        virtual ~i_symbol_load_callback() = default;

        i_symbol_load_callback(i_symbol_load_callback const&) = default;
        i_symbol_load_callback(i_symbol_load_callback &&) = default;

        i_symbol_load_callback& operator=(i_symbol_load_callback const&) = default;
        i_symbol_load_callback& operator=(i_symbol_load_callback &&) = default;

        [[nodiscard]] virtual bool deferred_symbol_load_cancel(std::wstring_view const& module_name) = 0;
        virtual void deferred_symbol_load_partial(std::wstring_view const& module_name) = 0;
        virtual void start_download(std::wstring_view const& module_name) = 0;
        virtual void download_percent(unsigned percent) = 0;
        virtual void download_complete() = 0;
        [[nodiscard]] virtual std::wostream& log_stream() const = 0;
        [[nodiscard]] virtual bool symbol_load_debug() const = 0;
        [[nodiscard]] virtual bool symbol_load_debug_memory() const = 0;
    };
}
