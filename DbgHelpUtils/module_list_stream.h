#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>
#include <limits>

#include "stream_module.h"

namespace dlg_help_utils
{
    class mini_dump;

    class module_list_stream
    {
    public:
        module_list_stream() = default;
        explicit module_list_stream(mini_dump const& dump, size_t index = std::numeric_limits<size_t>::max());

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] MINIDUMP_MODULE_LIST const& module_list() const { return *module_list_; }

        [[nodiscard]] std::vector<stream_module> const& list() const { return modules_; }

        [[nodiscard]] stream_module const* find_module(uint64_t address) const;
        [[nodiscard]] stream_module const* find_module(std::wstring_view const& module_name) const;

    private:
        [[nodiscard]] static MINIDUMP_MODULE_LIST const* get_module_list(mini_dump const& dump, size_t& index);
        [[nodiscard]] std::vector<stream_module> build_modules() const;

    private:
        mini_dump const* dump_{nullptr};
        MINIDUMP_MODULE_LIST const* module_list_{nullptr};
        size_t index_{};
        bool found_{false};
        std::vector<stream_module> const modules_{build_modules()};
    };
}
