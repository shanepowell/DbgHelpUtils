#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <optional>
#include <experimental/generator>
#include <limits>

#include "stream_thread_ex.h"

namespace dlg_help_utils
{
    class mini_dump;

    class thread_ex_list_stream
    {
    public:
        explicit thread_ex_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_THREAD_EX_LIST const& thread_list() const { return *thread_list_; }

        [[nodiscard]] std::experimental::generator<stream_thread_ex> list() const;

        [[nodiscard]] std::optional<stream_thread_ex> find_thread(ULONG32 thread_id,
                                                                  thread_names_list_stream const& names_list) const;

    private:
        mini_dump const& dump_;
        bool found_{false};
        size_t index_;
        MINIDUMP_THREAD_EX_LIST const* thread_list_{nullptr};
    };
}
