#pragma once

#define WIN32_LEAN_AND_MEAN
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>

namespace dlg_help_utils
{
    class mini_dump;

    class process_vm_counters_stream
    {
    public:
        explicit process_vm_counters_stream(mini_dump const& dump, size_t index = 0);

        [[nodiscard]] bool found() const { return found_; }
        [[nodiscard]] bool is_valid() const { return is_valid_; }
        [[nodiscard]] size_t index() const { return index_; }
        [[nodiscard]] size_t process_vm_counters_version() const { return process_vm_counters_version_; }

        [[nodiscard]] MINIDUMP_PROCESS_VM_COUNTERS_1 const& process_vm_counters() const
        {
            return *process_vm_counters_;
        }

        [[nodiscard]] MINIDUMP_PROCESS_VM_COUNTERS_2 const& process_vm_counters_2() const
        {
            return *process_vm_counters_2_;
        }

    private:
        bool found_{false};
        bool is_valid_{false};
        size_t index_{};
        size_t process_vm_counters_version_{};
        MINIDUMP_PROCESS_VM_COUNTERS_1 const* process_vm_counters_{nullptr};
        MINIDUMP_PROCESS_VM_COUNTERS_2 const* process_vm_counters_2_{nullptr};
    };
}
