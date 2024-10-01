#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "stream_thread_name.h"

namespace dlg_help_utils
{
    class thread_names_list_stream;

    class stream_thread_info
    {
    public:
        stream_thread_info() = default;
        explicit stream_thread_info(MINIDUMP_THREAD_INFO const& thread_info,
                                    thread_names_list_stream const& names_list);

        MINIDUMP_THREAD_INFO const* operator->() const { return thread_info_; }
        [[nodiscard]] std::wstring_view const& thread_name() const { return thread_name_.name(); }

    private:
        MINIDUMP_THREAD_INFO const* thread_info_{nullptr};
        stream_thread_name thread_name_;
    };
}
