#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>
#include <vector>

#include "stream_unloaded_module.h"

namespace dlg_help_utils
{
    class mini_dump;

    class unloaded_module_list_stream
    {
    public:
        explicit unloaded_module_list_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] ULONG32 size() const { return module_list_->NumberOfEntries; }

        [[nodiscard]] std::vector<stream_unloaded_module> const& list() const { return modules_; }

        [[nodiscard]] stream_unloaded_module const* find_module(uint64_t address) const;
        [[nodiscard]] stream_unloaded_module const* find_module(std::wstring_view const& module_name) const;

    private:
        [[nodiscard]] static MINIDUMP_UNLOADED_MODULE_LIST const* get_unloaded_module_list(mini_dump const& dump, size_t& index);
        [[nodiscard]] MINIDUMP_UNLOADED_MODULE const* get_unloaded_module() const;
        [[nodiscard]] std::vector<stream_unloaded_module> build_modules() const;

    private:
        mini_dump const& dump_;
        MINIDUMP_UNLOADED_MODULE_LIST const* module_list_;
        size_t const index_;
        bool const found_;
        bool const is_valid_;
        MINIDUMP_UNLOADED_MODULE const* list_{get_unloaded_module()};
        std::vector<stream_unloaded_module> const modules_{build_modules()};
    };
}
