#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <avrfsdk.h>
#include "generator.h"
#include <limits>

namespace dlg_help_utils
{
    class mini_dump;

    class handle_operation_list_stream
    {
    public:
        handle_operation_list_stream() = default;
        explicit handle_operation_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG32 size() const { return handle_operation_list_->NumberOfEntries; }

        [[nodiscard]] generator<AVRF_HANDLE_OPERATION const*> list() const;

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        MINIDUMP_HANDLE_OPERATION_LIST const* handle_operation_list_{nullptr};
        AVRF_HANDLE_OPERATION const* list_{nullptr};
    };
}
