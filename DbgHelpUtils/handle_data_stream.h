#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>
#include <map>
#include <experimental/generator>

#include "stream_handle.h"

namespace dlg_help_utils
{
    class mini_dump;

    class handle_data_stream
    {
    public:
        explicit handle_data_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] size_t handle_descriptor_version() const { return handle_descriptor_version_; }
        [[nodiscard]] ULONG64 size() const { return handle_data_list_->NumberOfDescriptors; }

        [[nodiscard]] std::map<std::wstring_view, size_t> const& handle_type_totals() const
        {
            return handle_type_totals_;
        }

        [[nodiscard]] std::experimental::generator<stream_handle> list() const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        bool is_valid_{false};
        size_t index_;
        size_t handle_descriptor_version_{0};
        MINIDUMP_HANDLE_DATA_STREAM const* handle_data_list_{nullptr};
        uint8_t const* list_{nullptr};
        std::map<std::wstring_view, size_t> handle_type_totals_;
    };
}
