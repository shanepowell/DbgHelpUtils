#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <optional>
#include <experimental/generator>

#include "stream_unloaded_module.h"

namespace dlg_help_utils
{
    class mini_dump;

    class unloaded_module_list_stream
    {
    public:
        explicit unloaded_module_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG32 size() const { return module_list_->NumberOfEntries; }

        [[nodiscard]] std::experimental::generator<stream_unloaded_module> list() const;

        [[nodiscard]] std::optional<stream_unloaded_module> find_module(uint64_t address) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        MINIDUMP_UNLOADED_MODULE_LIST const* module_list_{nullptr};
        MINIDUMP_UNLOADED_MODULE const* list_{nullptr};
    };
}
