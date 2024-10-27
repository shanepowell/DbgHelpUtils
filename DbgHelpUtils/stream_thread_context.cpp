#include "stream_thread_context.h"

#include "mini_dump.h"
#include "system_info_stream.h"

namespace dlg_help_utils
{
    stream_thread_context::stream_thread_context(mini_dump const& dump, MINIDUMP_LOCATION_DESCRIPTOR const& location)
    : context_{dump.rva32(location)}
    , size_{location.DataSize}
    {
        if (system_info_stream const system_info{dump}; system_info.found())
        {
            switch (system_info.system_info().ProcessorArchitecture)
            {
            case PROCESSOR_ARCHITECTURE_INTEL:
                x86_thread_context_available_ = true;
                x86_thread_context_ = static_cast<context_x86 const*>(context_);
                break;

            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
                wow64_thread_context_available_ = true;
                wow64_thread_context_ = static_cast<WOW64_CONTEXT const*>(context_);
                break;

            case PROCESSOR_ARCHITECTURE_AMD64:
                x64_thread_context_available_ = true;
                x64_thread_context_ = static_cast<context_x64 const*>(context_);
                break;

            default:
                break;
            }
        }
    }
}
