#pragma once

namespace dlg_help_utils::heap
{
    class process_heaps_options
    {
    public:
        [[nodiscard]] bool no_filter_heap_entries() const { return no_filter_heap_entries_; }
        [[nodiscard]] bool& no_filter_heap_entries() { return no_filter_heap_entries_; }

    private:
        bool no_filter_heap_entries_{false};
    };
}
