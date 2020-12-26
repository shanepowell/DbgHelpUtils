#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <avrfsdk.h>
#include <experimental/generator>

namespace dlg_help_utils
{
    class mini_dump;

    class handle_operation_list_stream
    {
    public:
        explicit handle_operation_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG64 size() const { return handle_operation_list_->NumberOfEntries; }

        [[nodiscard]] std::experimental::generator<AVRF_HANDLE_OPERATION const*> list() const;

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        MINIDUMP_HANDLE_OPERATION_LIST const* handle_operation_list_{nullptr};
        AVRF_HANDLE_OPERATION const* list_{nullptr};
    };
}
