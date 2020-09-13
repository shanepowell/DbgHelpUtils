#include "stream_thread_context.h"

#include "mini_dump.h"
#include "system_info_stream.h"

// ReSharper disable once CppInconsistentNaming
#define CONTEXT_i386 0x10000								// NOLINT(cppcoreguidelines-macro-usage)
#define CONTEXT_EXTENDED_REGISTERS	(CONTEXT_i386 | 0x20L)	// NOLINT(cppcoreguidelines-macro-usage)

namespace dlg_help_utils
{
    stream_thread_context::stream_thread_context(mini_dump const& dump, MINIDUMP_LOCATION_DESCRIPTOR const& location)
    {
        context_ = dump.rva32(location);
        size_ = location.DataSize;

        system_info_stream const system_info{dump};
        if (system_info.found())
        {
            switch (system_info.system_info().ProcessorArchitecture)
            {
            case PROCESSOR_ARCHITECTURE_INTEL:
                x86_thread_context_available_ = true;
                x86_thread_context_ = static_cast<WOW64_CONTEXT const*>(context_);
                x86_thread_context_has_extended_registers_ = (x86_thread_context_->ContextFlags &
                    CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS;
                break;

            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
                wow64_thread_context_available_ = true;
                wow64_thread_context_ = static_cast<WOW64_CONTEXT const*>(context_);
                wow64_thread_context_has_extended_registers_ = (wow64_thread_context_->ContextFlags &
                    CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS;
                break;

            case PROCESSOR_ARCHITECTURE_AMD64:
                x64_thread_context_available_ = true;
                x64_thread_context_ = static_cast<CONTEXT const*>(context_);
                break;

            default:
                break;
            }
        }
    }
}
