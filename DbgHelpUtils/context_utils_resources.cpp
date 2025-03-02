#include <map>

#include "context_utils.h"
#include "flags_string_utils.h"
#include "stream_thread_context.h"

#include "windows_setup.h"

using namespace std::string_literals;

namespace
{
    std::map<uint32_t, std::wstring> const x86_thread_context_flag_masks =
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

    std::map<uint32_t, std::wstring> const x64_thread_context_flag_masks =
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

    std::map<uint32_t, std::wstring> const wow64_thread_context_flag_masks =
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
    std::map<uint32_t, std::wstring> const mx_csr_register_masks =
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

    // https://en.wikipedia.org/wiki/FLAGS_register
    std::map<dlg_help_utils::flags_string_utils::option_mask<uint64_t>, std::wstring> const flags_register_masks =
    {
        {{.mask= 0x0001, .value= 0x0001}, L"carry flag (CF) - carry (CY)"s},
        {{.mask= 0x0001, .value= 0x0000}, L"carry flag (CF) - no cary (NC)"s},
        {{.mask= 0x0002, .value= std::nullopt}, L"Reserved"s},
        {{.mask= 0x0004, .value= 0x0004}, L"parity flag (PF) - parity even (PE)"s},
        {{.mask= 0x0004, .value= 0x0000}, L"parity flag (PF) - parity odd (PO)"s},
        {{.mask= 0x0008, .value= std::nullopt}, L"Reserved"s},
        {{.mask= 0x0010, .value= 0x0010}, L"auxiliary carry flag (AF) - auxiliary carry (AC)"s},
        {{.mask= 0x0010, .value= 0x0000}, L"auxiliary carry flag (AF) - no auxiliary carry (NA)"s},
        {{.mask= 0x0020, .value= std::nullopt}, L"Reserved"s},
        {{.mask= 0x0040, .value= 0x0040}, L"zero flag (ZF) - zero (ZR)"s},
        {{.mask= 0x0040, .value= 0x0000}, L"zero flag (ZF) - not zero (NZ)"s},
        {{.mask= 0x0080, .value= 0x0080}, L"sign flag (SF) - negative (NG)"s},
        {{.mask= 0x0080, .value= 0x0000}, L"sign flag (SF) - positive (PL)"s},
        {{.mask= 0x0100, .value= 0x0100}, L"trap flag (TF)"s},
        {{.mask= 0x0200, .value= 0x0200}, L"interrupt enable flag (IF) - enable interrupt (EI)"s},
        {{.mask= 0x0200, .value= 0x0000}, L"interrupt enable flag (IF) - disable interrupt (DI)"s},
        {{.mask= 0x0400, .value= 0x0400}, L"direction flag (DF) - down (DN)"s},
        {{.mask= 0x0400, .value= 0x0000}, L"direction flag (DF) - up (UP)"s},
        {{.mask= 0x0800, .value= 0x0800}, L"overflow flag (OF) - overflow (OV)"s},
        {{.mask= 0x0800, .value= 0x0000}, L"overflow flag (OF) - not overflow (NV)"s},
        {{.mask= 0x3000, .value= std::nullopt}, L"i/o privilege level"s},
        {{.mask= 0x4000, .value= std::nullopt}, L"nested task flag (NT)"s},
        {{.mask= 0x0002'0000, .value= 0x0002'0000}, L"virtual 8086 mode flag (VM)"s},
        {{.mask= 0x0004'0000, .value= 0x0004'0000}, L"alignment check (AC)"s},
        {{.mask= 0x0008'0000, .value= 0x0008'0000}, L"virtual interrupt flag (VIF)"s},
        {{.mask= 0x0010'0000, .value= 0x0010'0000}, L"virtual interrupt pending (VIP)"s},
        {{.mask= 0x0020'0000, .value= 0x0020'0000}, L"CPUID (ID)"s},
        {{.mask= 0x4000'0000, .value= 0x4000'0000}, L"AES key schedule loaded flag"s},
        {{.mask= 0x8000'0000, .value= 0x8000'0000}, L"alternate instruction set enabled"s},
    };

    // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf
    std::map<dlg_help_utils::flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_status_word_masks =
    {
        {{.mask= 0x0001, .value= static_cast<unsigned short>(0x0001)}, L"invalid operation (IE)"s},
        {{.mask= 0x0002, .value= static_cast<unsigned short>(0x0002)}, L"denormalized operand (DE)"s},
        {{.mask= 0x0004, .value= static_cast<unsigned short>(0x0004)}, L"zero divide (ZE)"s},
        {{.mask= 0x0008, .value= static_cast<unsigned short>(0x0008)}, L"overflow (OE)"s},
        {{.mask= 0x0010, .value= static_cast<unsigned short>(0x0010)}, L"underflow (UE)"s},
        {{.mask= 0x0020, .value= static_cast<unsigned short>(0x0020)}, L"precision (PE)"s},
        {{.mask= 0x0040, .value= static_cast<unsigned short>(0x0040)}, L"stack fault (SF)"s},
        {{.mask= 0x0080, .value= static_cast<unsigned short>(0x0080)}, L"error summary status (ES)"s},
        {{.mask= 0x0240, .value= static_cast<unsigned short>(0x0040)}, L"stack underflow"s},
        {{.mask= 0x0240, .value= static_cast<unsigned short>(0x0240)}, L"stack overflow"s},
        {{.mask= 0x0100, .value= static_cast<unsigned short>(0x0000)}, L"c0 - 0 (CF)"s},
        {{.mask= 0x0100, .value= static_cast<unsigned short>(0x0100)}, L"c0 - 1 (CF)"s},
        {{.mask= 0x0200, .value= static_cast<unsigned short>(0x0000)}, L"c1 - 0 (chopped)"s},
        {{.mask= 0x0200, .value= static_cast<unsigned short>(0x0200)}, L"c1 - 1 (rounded up)"s},
        {{.mask= 0x0400, .value= static_cast<unsigned short>(0x0000)}, L"c2 - 0 (PF - complete)"s},
        {{.mask= 0x0400, .value= static_cast<unsigned short>(0x0400)}, L"c2 - 1 (PF - incomplete)"s},
        {{.mask= 0x4000, .value= static_cast<unsigned short>(0x0000)}, L"c3 - 0 (ZF)"s},
        {{.mask= 0x4000, .value= static_cast<unsigned short>(0x4000)}, L"c3 - 1 (ZF)"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(0 << 11)}, L"ST0 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(1 << 11)}, L"ST1 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(2 << 11)}, L"ST2 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(3 << 11)}, L"ST3 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(4 << 11)}, L"ST4 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(5 << 11)}, L"ST5 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(6 << 11)}, L"ST6 is top of stack"s},
        {{.mask= 0x3800, .value= static_cast<unsigned short>(7 << 11)}, L"ST7 is top of stack"s},
        {{.mask= 0x8000, .value= static_cast<unsigned short>(0x8000)}, L"busy"s},
    };

    // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf
    std::map<dlg_help_utils::flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_control_word_masks =
    {
        {{.mask= 0x0001, .value= static_cast<unsigned short>(0x0001)}, L"invalid operation (IM)"s},
        {{.mask= 0x0002, .value= static_cast<unsigned short>(0x0002)}, L"denormalized operand (DM)"s},
        {{.mask= 0x0004, .value= static_cast<unsigned short>(0x0004)}, L"zero divide (ZM)"s},
        {{.mask= 0x0008, .value= static_cast<unsigned short>(0x0008)}, L"overflow (OM)"s},
        {{.mask= 0x0010, .value= static_cast<unsigned short>(0x0010)}, L"underflow (UM)"s},
        {{.mask= 0x0020, .value= static_cast<unsigned short>(0x0020)}, L"precision (PM)"s},
        {{.mask= 0x00C0, .value= std::nullopt}, L"reserved"s},
        {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0000)}, L"24 bits (single precision)"s},
        {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0100)}, L"reserved (precision)"s},
        {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0200)}, L"53 bits (double precision)"s},
        {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0300)}, L"64 bits (extended precision)"s},
        {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0000)}, L"round to nearest or even"s},
        {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0400)}, L"round down (toward -inf)"s},
        {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0800)}, L"round up (toward +inf)"s},
        {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0C00)}, L"chop (truncate toward zero)"s},
        {{.mask= 0x1000, .value= std::nullopt}, L"infinity control"s},
        {{.mask= 0xE000, .value= std::nullopt}, L"infinity control"s},
    };

    // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf
    std::map<dlg_help_utils::flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_tag_word_masks =
    {
        {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0000 <<  0)}, L"ST0 valid"s},
        {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0001 <<  0)}, L"ST0 zero"s},
        {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0002 <<  0)}, L"ST0 invalid or infinity"s},
        {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0003 <<  0)}, L"ST0 empty"s},
        {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0000 <<  1)}, L"ST1 valid"s},
        {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0001 <<  1)}, L"ST1 zero"s},
        {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0002 <<  1)}, L"ST1 invalid or infinity"s},
        {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0003 <<  1)}, L"ST1 empty"s},
        {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0000 <<  2)}, L"ST2 valid"s},
        {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0001 <<  2)}, L"ST2 zero"s},
        {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0002 <<  2)}, L"ST2 invalid or infinity"s},
        {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0003 <<  2)}, L"ST2 empty"s},
        {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0000 <<  3)}, L"ST3 valid"s},
        {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0001 <<  3)}, L"ST3 zero"s},
        {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0002 <<  3)}, L"ST3 invalid or infinity"s},
        {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0003 <<  3)}, L"ST3 empty"s},
        {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0000 <<  4)}, L"ST4 valid"s},
        {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0001 <<  4)}, L"ST4 zero"s},
        {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0002 <<  4)}, L"ST4 invalid or infinity"s},
        {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0003 <<  4)}, L"ST4 empty"s},
        {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0000 <<  5)}, L"ST5 valid"s},
        {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0001 <<  5)}, L"ST5 zero"s},
        {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0002 <<  5)}, L"ST5 invalid or infinity"s},
        {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0003 <<  5)}, L"ST5 empty"s},
        {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0000 <<  6)}, L"ST6 valid"s},
        {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0001 <<  6)}, L"ST6 zero"s},
        {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0002 <<  6)}, L"ST6 invalid or infinity"s},
        {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0003 <<  6)}, L"ST6 empty"s},
        {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0000 <<  7)}, L"ST7 valid"s},
        {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0001 <<  7)}, L"ST7 zero"s},
        {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0002 <<  7)}, L"ST7 invalid or infinity"s},
        {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0003 <<  7)}, L"ST7 empty"s},
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

    std::wstring get_flags_register_to_string(uint64_t const flags)
    {
        return generate_flags_string(flags, flags_register_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_status_word_to_string(uint16_t const status_word)
    {
        return generate_flags_string(status_word, npx_status_word_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_control_word_to_string(uint16_t const control_word)
    {
        return generate_flags_string(control_word, npx_control_word_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_tag_word_to_string(uint16_t const tag_word)
    {
        return generate_flags_string(tag_word, npx_tag_word_masks, flags_string_utils::mask_used_flags_t{false});
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

    std::vector<std::wstring> get_flags_register(uint64_t const flags)
    {
        return generate_flags_strings(flags, flags_register_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_status_word(uint16_t const status_word)
    {
        return generate_flags_strings(status_word, npx_status_word_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_control_word(uint16_t const control_word)
    {
        return generate_flags_strings(control_word, npx_control_word_masks, flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_tag_word(uint16_t const tag_word)
    {
        return generate_flags_strings(tag_word, npx_control_word_masks, flags_string_utils::mask_used_flags_t{false});
    }
}