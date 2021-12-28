#include "register_names.h"

#include <map>

using namespace std::string_view_literals;

namespace dlg_help_utils::register_names
{
    namespace
    {
       std::map<uint32_t, std::wstring_view> const g_register_names =
        {
            {static_cast<uint32_t>(CV_AMD64_AL), L"AL"sv},
            {static_cast<uint32_t>(CV_AMD64_BL), L"BL"sv},
            {static_cast<uint32_t>(CV_AMD64_CL), L"CL"sv},
            {static_cast<uint32_t>(CV_AMD64_DL), L"DL"sv},
            {static_cast<uint32_t>(CV_AMD64_AX), L"AX"sv},
            {static_cast<uint32_t>(CV_AMD64_BX), L"BX"sv},
            {static_cast<uint32_t>(CV_AMD64_CX), L"CX"sv},
            {static_cast<uint32_t>(CV_AMD64_DX), L"DX"sv},
            {static_cast<uint32_t>(CV_AMD64_SP), L"SP"sv},
            {static_cast<uint32_t>(CV_AMD64_BP), L"BP"sv},
            {static_cast<uint32_t>(CV_AMD64_SI), L"SI"sv},
            {static_cast<uint32_t>(CV_AMD64_DI), L"DI"sv},
            {static_cast<uint32_t>(CV_AMD64_EAX), L"EAX"sv},
            {static_cast<uint32_t>(CV_AMD64_EBX), L"EBX"sv},
            {static_cast<uint32_t>(CV_AMD64_ECX), L"ECX"sv},
            {static_cast<uint32_t>(CV_AMD64_EDX), L"EDX"sv},
            {static_cast<uint32_t>(CV_AMD64_ESP), L"ESP"sv},
            {static_cast<uint32_t>(CV_AMD64_EBP), L"EBP"sv},
            {static_cast<uint32_t>(CV_AMD64_ESI), L"ESI"sv},
            {static_cast<uint32_t>(CV_AMD64_EDI), L"EDI"sv},
            {static_cast<uint32_t>(CV_AMD64_RAX), L"RAX"sv},
            {static_cast<uint32_t>(CV_AMD64_RBX), L"RBX"sv},
            {static_cast<uint32_t>(CV_AMD64_RCX), L"RCX"sv},
            {static_cast<uint32_t>(CV_AMD64_RDX), L"RDX"sv},
            {static_cast<uint32_t>(CV_AMD64_RSP), L"RSP"sv},
            {static_cast<uint32_t>(CV_AMD64_RBP), L"RBP"sv},
            {static_cast<uint32_t>(CV_AMD64_RSI), L"RSI"sv},
            {static_cast<uint32_t>(CV_AMD64_RDI), L"RDI"sv},
            {static_cast<uint32_t>(CV_AMD64_R8B), L"R8B"sv},
            {static_cast<uint32_t>(CV_AMD64_R9B), L"R9B"sv},
            {static_cast<uint32_t>(CV_AMD64_R10B), L"R10B"sv},
            {static_cast<uint32_t>(CV_AMD64_R11B), L"R11B"sv},
            {static_cast<uint32_t>(CV_AMD64_R12B), L"R12B"sv},
            {static_cast<uint32_t>(CV_AMD64_R13B), L"R13B"sv},
            {static_cast<uint32_t>(CV_AMD64_R14B), L"R14B"sv},
            {static_cast<uint32_t>(CV_AMD64_R15B), L"R15B"sv},
            {static_cast<uint32_t>(CV_AMD64_R8W), L"R8W"sv},
            {static_cast<uint32_t>(CV_AMD64_R9W), L"R9W"sv},
            {static_cast<uint32_t>(CV_AMD64_R10W), L"R10W"sv},
            {static_cast<uint32_t>(CV_AMD64_R11W), L"R11W"sv},
            {static_cast<uint32_t>(CV_AMD64_R12W), L"R12W"sv},
            {static_cast<uint32_t>(CV_AMD64_R13W), L"R13W"sv},
            {static_cast<uint32_t>(CV_AMD64_R14W), L"R14W"sv},
            {static_cast<uint32_t>(CV_AMD64_R15W), L"R15W"sv},
            {static_cast<uint32_t>(CV_AMD64_R8D), L"R8D"sv},
            {static_cast<uint32_t>(CV_AMD64_R9D), L"R9D"sv},
            {static_cast<uint32_t>(CV_AMD64_R10D), L"R10D"sv},
            {static_cast<uint32_t>(CV_AMD64_R11D), L"R11D"sv},
            {static_cast<uint32_t>(CV_AMD64_R12D), L"R12D"sv},
            {static_cast<uint32_t>(CV_AMD64_R13D), L"R13D"sv},
            {static_cast<uint32_t>(CV_AMD64_R14D), L"R14D"sv},
            {static_cast<uint32_t>(CV_AMD64_R15D), L"R15D"sv},
            {static_cast<uint32_t>(CV_AMD64_R8), L"R8"sv},
            {static_cast<uint32_t>(CV_AMD64_R9), L"R9"sv},
            {static_cast<uint32_t>(CV_AMD64_R10), L"R10"sv},
            {static_cast<uint32_t>(CV_AMD64_R11), L"R11"sv},
            {static_cast<uint32_t>(CV_AMD64_R12), L"R12"sv},
            {static_cast<uint32_t>(CV_AMD64_R13), L"R13"sv},
            {static_cast<uint32_t>(CV_AMD64_R14), L"R14"sv},
            {static_cast<uint32_t>(CV_AMD64_R15), L"R15"sv},
        };
    }

    std::wstring_view get_register_name(CV_HREG_e const register_type)
    {
        if (auto const it = g_register_names.find(static_cast<uint32_t>(register_type)); it != g_register_names.end())
        {
            return it->second;
        }

        return L"unknown"sv;
    }
}
