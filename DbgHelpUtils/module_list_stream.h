#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <optional>
#include <experimental/generator>

#include "stream_module.h"

namespace dlg_help_utils
{
    class mini_dump;

    class module_list_stream
    {
    public:
        explicit module_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MODULE_LIST const& module_list() const { return *module_list_; }

        [[nodiscard]] std::experimental::generator<stream_module> list() const;

        [[nodiscard]] std::optional<stream_module> find_module(uint64_t address) const;
        [[nodiscard]] std::optional<stream_module> find_module(std::wstring_view const& module_name) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        size_t index_;
        MINIDUMP_MODULE_LIST const* module_list_{nullptr};
    };
}
