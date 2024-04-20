#include "pch.h"
#include "DbgHelpUtils/register_names.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"

using namespace std::string_literals;

namespace dlg_help_utils::register_names
{
    namespace
    {
        std::unordered_map<CV_HREG_e, std::wstring> const& register_names()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
            static std::unordered_map<CV_HREG_e, std::wstring> const register_names =
            {
                {CV_AMD64_AL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/{CV_AMD64_AL").ValueAsString()}},
                {CV_AMD64_BL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_BL").ValueAsString()}},
                {CV_AMD64_CL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_CL").ValueAsString()}},
                {CV_AMD64_DL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_DL").ValueAsString()}},
                {CV_AMD64_AX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_AX").ValueAsString()}},
                {CV_AMD64_BX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_BX").ValueAsString()}},
                {CV_AMD64_CX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_CX").ValueAsString()}},
                {CV_AMD64_DX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_DX").ValueAsString()}},
                {CV_AMD64_SP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_SP").ValueAsString()}},
                {CV_AMD64_BP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_BP").ValueAsString()}},
                {CV_AMD64_SI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_SI").ValueAsString()}},
                {CV_AMD64_DI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_DI").ValueAsString()}},
                {CV_AMD64_EAX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_EAX").ValueAsString()}},
                {CV_AMD64_EBX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_EBX").ValueAsString()}},
                {CV_AMD64_ECX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_ECX").ValueAsString()}},
                {CV_AMD64_EDX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_EDX").ValueAsString()}},
                {CV_AMD64_ESP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_ESP").ValueAsString()}},
                {CV_AMD64_EBP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_EBP").ValueAsString()}},
                {CV_AMD64_ESI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_ESI").ValueAsString()}},
                {CV_AMD64_EDI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_EDI").ValueAsString()}},
                {CV_AMD64_RAX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RAX").ValueAsString()}},
                {CV_AMD64_RBX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RBX").ValueAsString()}},
                {CV_AMD64_RCX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RCX").ValueAsString()}},
                {CV_AMD64_RDX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RDX").ValueAsString()}},
                {CV_AMD64_RSP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RSP").ValueAsString()}},
                {CV_AMD64_RBP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RBP").ValueAsString()}},
                {CV_AMD64_RSI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RSI").ValueAsString()}},
                {CV_AMD64_RDI, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_RDI").ValueAsString()}},
                {CV_AMD64_R8B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R8B").ValueAsString()}},
                {CV_AMD64_R9B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R9B").ValueAsString()}},
                {CV_AMD64_R10B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R10B").ValueAsString()}},
                {CV_AMD64_R11B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R11B").ValueAsString()}},
                {CV_AMD64_R12B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R12B").ValueAsString()}},
                {CV_AMD64_R13B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R13B").ValueAsString()}},
                {CV_AMD64_R14B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R14B").ValueAsString()}},
                {CV_AMD64_R15B, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R15B").ValueAsString()}},
                {CV_AMD64_R8W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R8W").ValueAsString()}},
                {CV_AMD64_R9W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R9W").ValueAsString()}},
                {CV_AMD64_R10W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R10W").ValueAsString()}},
                {CV_AMD64_R11W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R11W").ValueAsString()}},
                {CV_AMD64_R12W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R12W").ValueAsString()}},
                {CV_AMD64_R13W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R13W").ValueAsString()}},
                {CV_AMD64_R14W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R14W").ValueAsString()}},
                {CV_AMD64_R15W, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R15W").ValueAsString()}},
                {CV_AMD64_R8D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R8D").ValueAsString()}},
                {CV_AMD64_R9D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R9D").ValueAsString()}},
                {CV_AMD64_R10D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R10D").ValueAsString()}},
                {CV_AMD64_R11D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R11D").ValueAsString()}},
                {CV_AMD64_R12D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R12D").ValueAsString()}},
                {CV_AMD64_R13D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R13D").ValueAsString()}},
                {CV_AMD64_R14D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R14D").ValueAsString()}},
                {CV_AMD64_R15D, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R15D").ValueAsString()}},
                {CV_AMD64_R8, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R8").ValueAsString()}},
                {CV_AMD64_R9, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R9").ValueAsString()}},
                {CV_AMD64_R10, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R10").ValueAsString()}},
                {CV_AMD64_R11, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R11").ValueAsString()}},
                {CV_AMD64_R12, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R12").ValueAsString()}},
                {CV_AMD64_R13, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R13").ValueAsString()}},
                {CV_AMD64_R14, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R14").ValueAsString()}},
                {CV_AMD64_R15, std::wstring{rm.MainResourceMap().GetValue(L"Resources/CV_AMD64_R15").ValueAsString()}},
            };

            return register_names;
        }
    }

    std::wstring get_register_name(CV_HREG_e const register_type)
    {
        return flags_string_utils::generate_enum_string(register_type, register_names());
    }
}
