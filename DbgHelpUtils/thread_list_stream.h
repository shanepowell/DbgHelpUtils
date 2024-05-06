#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <optional>
#include <experimental/generator>
#include <limits>

#include "stream_thread.h"

namespace dlg_help_utils
{
    class mini_dump;

    class thread_list_stream
    {
    public:
        thread_list_stream() = default;
        explicit thread_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_THREAD_LIST const& thread_list() const { return *thread_list_; }

        [[nodiscard]] std::experimental::generator<stream_thread> list() const;

        [[nodiscard]] std::optional<stream_thread> find_thread(ULONG32 thread_id,
                                                               thread_names_list_stream const& names_list,
                                                               memory_list_stream const& memory_list,
                                                               memory64_list_stream const& memory64_list) const;

    private:
        mini_dump const* dump_{nullptr};
        bool found_{false};
        size_t index_{};
        MINIDUMP_THREAD_LIST const* thread_list_{nullptr};
    };
}
