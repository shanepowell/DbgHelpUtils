#pragma once

namespace dlg_help_utils
{
    class mini_dump;
}

void dump_mini_dump_system_info_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_misc_info_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_process_vm_counters_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
