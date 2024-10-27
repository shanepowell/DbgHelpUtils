#include <unordered_map>

#include "context_utils.h"
#include "flags_string_utils.h"
#include "stream_thread_context.h"

#include "windows_setup.h"

using namespace std::string_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const x86_thread_context_flag_masks =
    {
        {X86_CONTEXT_i386, L"i386/i486 context"s},
        {X86_CONTEXT_CONTROL, L"control segments"s},
        {X86_CONTEXT_INTEGER, L"integer registers"s},
        {X86_CONTEXT_SEGMENTS, L"segments"s},
        {X86_CONTEXT_FLOATING_POINT, L"floating point registers"s},
        {X86_CONTEXT_DEBUG_REGISTERS, L"debug registers"s},
        {X86_CONTEXT_EXTENDED_REGISTERS, L"extended registers"s},
        {X86_CONTEXT_FULL, L"full context"s},
        {X86_CONTEXT_ALL, L"all context"s},
        {X86_CONTEXT_XSTATE, L"xstate"s},
        {X86_CONTEXT_EXCEPTION_ACTIVE, L"exception active"s},
        {X86_CONTEXT_SERVICE_ACTIVE, L"service active"s},
        {X86_CONTEXT_EXCEPTION_REQUEST, L"exception request"s},
        {X86_CONTEXT_EXCEPTION_REPORTING, L"exception reporting"s},
    };

    std::unordered_map<uint32_t, std::wstring> const x64_thread_context_flag_masks =
    {
        {X64_CONTEXT_AMD64, L"amd64 context"s},
        {X64_CONTEXT_CONTROL, L"control segments"s},
        {X64_CONTEXT_INTEGER, L"integer registers"s},
        {X64_CONTEXT_SEGMENTS, L"segments"s},
        {X64_CONTEXT_FLOATING_POINT, L"floating point registers"s},
        {X64_CONTEXT_DEBUG_REGISTERS, L"debug registers"s},
        {X64_CONTEXT_FULL, L"full context"s},
        {X64_CONTEXT_ALL, L"all context"s},
        {X64_CONTEXT_XSTATE, L"xstate"s},
        {X64_CONTEXT_KERNEL_CET, L"kernel cet"s},
        {X64_CONTEXT_EXCEPTION_ACTIVE, L"exception active"s},
        {X64_CONTEXT_SERVICE_ACTIVE, L"service active"s},
        {X64_CONTEXT_EXCEPTION_REQUEST, L"exception request"s},
        {X64_CONTEXT_EXCEPTION_REPORTING, L"exception reporting"s},
    };

    std::unordered_map<uint32_t, std::wstring> const wow64_thread_context_flag_masks =
    {
        {WOW64_CONTEXT_i386, L"i386/i486 context"s},
        {WOW64_CONTEXT_CONTROL, L"control segments"s},
        {WOW64_CONTEXT_INTEGER, L"integer registers"s},
        {WOW64_CONTEXT_SEGMENTS, L"segments"s},
        {WOW64_CONTEXT_FLOATING_POINT, L"floating point registers"s},
        {WOW64_CONTEXT_DEBUG_REGISTERS, L"debug registers"s},
        {WOW64_CONTEXT_EXTENDED_REGISTERS, L"extended registers"s},
        {WOW64_CONTEXT_FULL, L"full context"s},
        {WOW64_CONTEXT_ALL, L"all context"s},
        {WOW64_CONTEXT_XSTATE, L"xstate"s},
        {WOW64_CONTEXT_EXCEPTION_ACTIVE, L"exception active"s},
        {WOW64_CONTEXT_SERVICE_ACTIVE, L"service active"s},
        {WOW64_CONTEXT_EXCEPTION_REQUEST, L"exception request"s},
        {WOW64_CONTEXT_EXCEPTION_REPORTING, L"exception reporting"s},
    };

    // ReSharper disable CommentTypo
    // https://help.totalview.io/previous_releases/2024.2/HTML/index.html#page/TotalView/Intelx86MXSCRRegister.html
    // ReSharper restore CommentTypo
    std::unordered_map<uint32_t, std::wstring> const mx_csr_register_masks =
    {
        {0x8000, L"flush to zero (FZ)"s},
        {0x0000, L"to nearest rounding mode (RC=RN)"s},
        {0x2000, L"toward negative infinity rounding mode (RC=R-)"s},
        {0x4000, L"toward positive infinity rounding mode (RC=R+)"s},
        {0x6000, L"toward zero rounding mode (RC=RZ)"s},
        {	0x1000, L"precision mask (EM=PM)"s},
        {	0x0800, L"underflow mask (EM=UM)"s},
        {	0x0400, L"overflow mask (EM=OM)"s},
        {	0x0200, L"divide-by-zero mask (EM=ZM)"s},
        {	0x0100, L"denormal mask (EM=DM)"s},
        {	0x0080, L"invalid operation mask (EM=IM)"s},
        {	0x0040, L"denormals are zeros (DAZ)"s},
        {	0x0020, L"precision flag (EF=PE)"s},
        {	0x0010, L"underflow flag (EF=UE)"s},
        {	0x0008, L"overflow flag (EF=OE)"s},
        {	0x0004, L"divide-by-zero flag (EF=ZE)"s},
        {	0x0002, L"denormal flag (EF=DE)"s},
        {	0x0001, L"invalid operation flag (EF=IE)"s},
    };
}


namespace dlg_help_utils::context_utils::resources
{
    std::wstring get_x86_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, x86_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_x64_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, x64_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_wow64_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, wow64_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_mx_csr_register_to_string(uint32_t const mx_csr)
    {
        return generate_flags_string(mx_csr, mx_csr_register_masks, flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> get_x86_thread_context_flags(uint32_t const context_flags)
    {
        return generate_flags_strings(context_flags, x86_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_x64_thread_context_flags(uint32_t const context_flags)
    {
        return generate_flags_strings(context_flags, x64_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_wow64_thread_context_flags(uint32_t const context_flags)
    {
        return generate_flags_strings(context_flags, wow64_thread_context_flag_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_mx_csr_register(uint32_t const mx_csr)
    {
        return generate_flags_strings(mx_csr, mx_csr_register_masks, flags_string_utils::mask_used_flags_t{true});
    }
}