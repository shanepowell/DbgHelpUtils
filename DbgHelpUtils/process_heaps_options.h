#pragma once

namespace dlg_help_utils::heap
{
    class process_heaps_options
    {
    public:
        [[nodiscard]] bool no_filter_heap_entries() const { return no_filter_heap_entries_; }
        [[nodiscard]] bool& no_filter_heap_entries() { return no_filter_heap_entries_; }

        [[nodiscard]] bool mark_system_heap_entries_children_as_system() const { return mark_system_heap_entries_children_as_system_; }
        [[nodiscard]] bool& mark_system_heap_entries_children_as_system() { return mark_system_heap_entries_children_as_system_; }

    private:
        bool no_filter_heap_entries_{false};
        bool mark_system_heap_entries_children_as_system_{false};
    };
}
