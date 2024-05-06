#include "system_info_stream.h"

#include "mini_dump.h"
#include "string_conversation.h"
#include "string_stream.h"

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    system_info_stream::system_info_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(SystemInfoStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        system_info_ = static_cast<MINIDUMP_SYSTEM_INFO const*>(dump.rva32(entry->Location));

        if (system_info_->CSDVersionRva != 0)
        {
            csd_version_ = string_stream::to_string(dump, system_info_->CSDVersionRva);
        }

        if(has_x86_cpu_info())
        {
            vendor_id_ = string_conversation::acp_to_wstring(std::string_view{
                reinterpret_cast<char const*>(system_info_->Cpu.X86CpuInfo.VendorId),
                sizeof(system_info_->Cpu.X86CpuInfo.VendorId)
            });
            is_intel_ = vendor_id_ == L"GenuineIntel"sv;
            is_amd_ = vendor_id_ == L"AuthenticAMD"sv;
        }

        if ((system_info_->ProcessorRevision & 0xFF00) == 0xFF00)
        {
            processor_model_ = (system_info_->ProcessorRevision & 0x00F0) >> 4;
            processor_stepping_ = system_info_->ProcessorRevision & 0x000F;
        }
        else
        {
            processor_model_ = (system_info_->ProcessorRevision & 0xFF00) >> 8;
            processor_stepping_ = system_info_->ProcessorRevision & 0x00FF;
        }

        found_ = true;
    }

    bool system_info_stream::is_x86() const
    {
        if (found())
        {
            switch (system_info().ProcessorArchitecture)
            {
            case PROCESSOR_ARCHITECTURE_INTEL:
            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
                return true;

            default:
                break;
            }
        }

        return false;
    }

    bool system_info_stream::is_x64() const
    {
        return found() && system_info().ProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
    }

    bool system_info_stream::has_x86_cpu_info() const
    {
        return system_info().ProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL;
    }
}
