#include "process_vm_counters_stream.h"

#include "mini_dump.h"
#include "string_conversation.h"
#include "string_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    process_vm_counters_stream::process_vm_counters_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(ProcessVmCountersStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        auto const* data = dump.rva32(entry->Location);
        found_ = true;

        if (entry->Location.DataSize >= sizeof(MINIDUMP_PROCESS_VM_COUNTERS_1))
        {
            process_vm_counters_version_ = 1;
            process_vm_counters_ = static_cast<MINIDUMP_PROCESS_VM_COUNTERS_1 const*>(data);
            is_valid_ = true;
        }
        else
        {
            return;
        }

        if (entry->Location.DataSize >= sizeof(MINIDUMP_PROCESS_VM_COUNTERS_2))
        {
            process_vm_counters_version_ = 2;
            process_vm_counters_2_ = static_cast<MINIDUMP_PROCESS_VM_COUNTERS_2 const*>(data);
        }
    }
}
