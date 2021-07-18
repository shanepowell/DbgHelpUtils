#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include <string_view>
#include <experimental/generator>

#include "stream_handle_object_information.h"

namespace dlg_help_utils
{
    class mini_dump;

    class stream_handle
    {
    public:
        explicit stream_handle(mini_dump const& dump, void const* data, size_t handle_descriptor_version);

        [[nodiscard]] size_t handle_descriptor_version() const { return handle_descriptor_version_; }

        [[nodiscard]] MINIDUMP_HANDLE_DESCRIPTOR const& descriptor() const
        {
            return *static_cast<MINIDUMP_HANDLE_DESCRIPTOR const*>(data_);
        }

        [[nodiscard]] MINIDUMP_HANDLE_DESCRIPTOR_2 const& descriptor_2() const
        {
            return *static_cast<MINIDUMP_HANDLE_DESCRIPTOR_2 const*>(data_);
        }

        [[nodiscard]] std::wstring_view const& type_name() const { return type_name_; }
        [[nodiscard]] std::wstring_view const& object_name() const { return object_name_; }

        [[nodiscard]] std::experimental::generator<stream_handle_object_information> list() const;

    private:
        mini_dump const& dump_;
        void const* data_;
        size_t handle_descriptor_version_;
        std::wstring_view type_name_;
        std::wstring_view object_name_;
    };
}
