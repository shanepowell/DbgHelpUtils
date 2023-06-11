#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <experimental/generator>

#include "stream_thread_name.h"

namespace dlg_help_utils
{
    class mini_dump;

    class thread_names_list_stream
    {
    public:
        explicit thread_names_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_THREAD_NAME_LIST const& thread_names_list() const { return *thread_name_list_; }

        [[nodiscard]] std::experimental::generator<stream_thread_name> list() const;
        [[nodiscard]] stream_thread_name get_thread_name_for_thread_id(uint32_t thread_id) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        size_t index_;
        MINIDUMP_THREAD_NAME_LIST const* thread_name_list_{nullptr};
    };
}
