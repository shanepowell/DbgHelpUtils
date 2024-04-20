#include "register_names.h"

#include "flags_string_utils.h"

using namespace std::string_literals;

namespace dlg_help_utils::register_names
{
    namespace
    {
       std::unordered_map<CV_HREG_e, std::wstring> const g_register_names =
        {
            {CV_AMD64_AL, L"AL"s},
            {CV_AMD64_BL, L"BL"s},
            {CV_AMD64_CL, L"CL"s},
            {CV_AMD64_DL, L"DL"s},
            {CV_AMD64_AX, L"AX"s},
            {CV_AMD64_BX, L"BX"s},
            {CV_AMD64_CX, L"CX"s},
            {CV_AMD64_DX, L"DX"s},
            {CV_AMD64_SP, L"SP"s},
            {CV_AMD64_BP, L"BP"s},
            {CV_AMD64_SI, L"SI"s},
            {CV_AMD64_DI, L"DI"s},
            {CV_AMD64_EAX, L"EAX"s},
            {CV_AMD64_EBX, L"EBX"s},
            {CV_AMD64_ECX, L"ECX"s},
            {CV_AMD64_EDX, L"EDX"s},
            {CV_AMD64_ESP, L"ESP"s},
            {CV_AMD64_EBP, L"EBP"s},
            {CV_AMD64_ESI, L"ESI"s},
            {CV_AMD64_EDI, L"EDI"s},
            {CV_AMD64_RAX, L"RAX"s},
            {CV_AMD64_RBX, L"RBX"s},
            {CV_AMD64_RCX, L"RCX"s},
            {CV_AMD64_RDX, L"RDX"s},
            {CV_AMD64_RSP, L"RSP"s},
            {CV_AMD64_RBP, L"RBP"s},
            {CV_AMD64_RSI, L"RSI"s},
            {CV_AMD64_RDI, L"RDI"s},
            {CV_AMD64_R8B, L"R8B"s},
            {CV_AMD64_R9B, L"R9B"s},
            {CV_AMD64_R10B, L"R10B"s},
            {CV_AMD64_R11B, L"R11B"s},
            {CV_AMD64_R12B, L"R12B"s},
            {CV_AMD64_R13B, L"R13B"s},
            {CV_AMD64_R14B, L"R14B"s},
            {CV_AMD64_R15B, L"R15B"s},
            {CV_AMD64_R8W, L"R8W"s},
            {CV_AMD64_R9W, L"R9W"s},
            {CV_AMD64_R10W, L"R10W"s},
            {CV_AMD64_R11W, L"R11W"s},
            {CV_AMD64_R12W, L"R12W"s},
            {CV_AMD64_R13W, L"R13W"s},
            {CV_AMD64_R14W, L"R14W"s},
            {CV_AMD64_R15W, L"R15W"s},
            {CV_AMD64_R8D, L"R8D"s},
            {CV_AMD64_R9D, L"R9D"s},
            {CV_AMD64_R10D, L"R10D"s},
            {CV_AMD64_R11D, L"R11D"s},
            {CV_AMD64_R12D, L"R12D"s},
            {CV_AMD64_R13D, L"R13D"s},
            {CV_AMD64_R14D, L"R14D"s},
            {CV_AMD64_R15D, L"R15D"s},
            {CV_AMD64_R8, L"R8"s},
            {CV_AMD64_R9, L"R9"s},
            {CV_AMD64_R10, L"R10"s},
            {CV_AMD64_R11, L"R11"s},
            {CV_AMD64_R12, L"R12"s},
            {CV_AMD64_R13, L"R13"s},
            {CV_AMD64_R14, L"R14"s},
            {CV_AMD64_R15, L"R15"s},
        };
    }

    std::wstring get_register_name(CV_HREG_e const register_type)
    {
        return flags_string_utils::generate_enum_string(register_type, g_register_names);
    }
}
