#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <map>
#include <experimental/generator>
#include <limits>

#include "stream_handle.h"

namespace dlg_help_utils
{
    class mini_dump;

    class handle_data_stream
    {
    public:
        handle_data_stream() = default;
        explicit handle_data_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] uint32_t handle_descriptor_version() const { return handle_descriptor_version_; }
        [[nodiscard]] uint32_t size() const { return handle_data_list_->NumberOfDescriptors; }

        [[nodiscard]] std::map<std::wstring_view, uint32_t> const& handle_type_totals() const
        {
            return handle_type_totals_;
        }

        [[nodiscard]] std::experimental::generator<stream_handle> list() const;

    private:
        mini_dump const* dump_{nullptr};
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        uint32_t handle_descriptor_version_{0};
        MINIDUMP_HANDLE_DATA_STREAM const* handle_data_list_{nullptr};
        uint8_t const* list_{nullptr};
        std::map<std::wstring_view, uint32_t> handle_type_totals_;
    };
}
