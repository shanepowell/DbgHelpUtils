#include "pch.h"
#include "DbgHelpUtils/context_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/stream_thread_context.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace std::string_literals;

namespace dlg_help_utils::context_utils::resources
{
    namespace
    {
        std::map<uint32_t, std::wstring> const& x86_thread_context_flag_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            static std::map<uint32_t, std::wstring> const x86_thread_context_flag_masks =
            {
                {X86_CONTEXT_i386, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_i386").ValueAsString()}},
                {X86_CONTEXT_CONTROL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_CONTROL").ValueAsString()}},
                {X86_CONTEXT_INTEGER, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_INTEGER").ValueAsString()}},
                {X86_CONTEXT_SEGMENTS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_SEGMENTS").ValueAsString()}},
                {X86_CONTEXT_FLOATING_POINT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_FLOATING_POINT").ValueAsString()}},
                {X86_CONTEXT_DEBUG_REGISTERS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_DEBUG_REGISTERS").ValueAsString()}},
                {X86_CONTEXT_EXTENDED_REGISTERS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_EXTENDED_REGISTERS").ValueAsString()}},
                {X86_CONTEXT_FULL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_FULL").ValueAsString()}},
                {X86_CONTEXT_ALL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_ALL").ValueAsString()}},
                {X86_CONTEXT_XSTATE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_XSTATE").ValueAsString()}},
                {X86_CONTEXT_EXCEPTION_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_EXCEPTION_ACTIVE").ValueAsString()}},
                {X86_CONTEXT_SERVICE_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_SERVICE_ACTIVE").ValueAsString()}},
                {X86_CONTEXT_EXCEPTION_REQUEST, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_EXCEPTION_REQUEST").ValueAsString()}},
                {X86_CONTEXT_EXCEPTION_REPORTING, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X86_CONTEXT_EXCEPTION_REPORTING").ValueAsString()}},
            };

            return x86_thread_context_flag_masks;
        }

        std::map<uint32_t, std::wstring> const& x64_thread_context_flag_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            static std::map<uint32_t, std::wstring> const x64_thread_context_flag_masks =
            {
                {X64_CONTEXT_AMD64, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_AMD64").ValueAsString()}},
                {X64_CONTEXT_CONTROL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_CONTROL").ValueAsString()}},
                {X64_CONTEXT_INTEGER, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_INTEGER").ValueAsString()}},
                {X64_CONTEXT_SEGMENTS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_SEGMENTS").ValueAsString()}},
                {X64_CONTEXT_FLOATING_POINT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_FLOATING_POINT").ValueAsString()}},
                {X64_CONTEXT_DEBUG_REGISTERS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_DEBUG_REGISTERS").ValueAsString()}},
                {X64_CONTEXT_FULL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_FULL").ValueAsString()}},
                {X64_CONTEXT_ALL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_ALL").ValueAsString()}},
                {X64_CONTEXT_XSTATE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_XSTATE").ValueAsString()}},
                {X64_CONTEXT_KERNEL_CET, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_KERNEL_CET").ValueAsString()}},
                {X64_CONTEXT_EXCEPTION_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_EXCEPTION_ACTIVE").ValueAsString()}},
                {X64_CONTEXT_SERVICE_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_SERVICE_ACTIVE").ValueAsString()}},
                {X64_CONTEXT_EXCEPTION_REQUEST, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_EXCEPTION_REQUEST").ValueAsString()}},
                {X64_CONTEXT_EXCEPTION_REPORTING, std::wstring{rm.MainResourceMap().GetValue(L"Resources/X64_CONTEXT_EXCEPTION_REPORTING").ValueAsString()}},
            };

            return x64_thread_context_flag_masks;
        }

        std::map<uint32_t, std::wstring> const& wow64_thread_context_flag_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            static std::map<uint32_t, std::wstring> const wow64_thread_context_flag_masks =
            {
                {WOW64_CONTEXT_i386, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_i386").ValueAsString()}}, 
                {WOW64_CONTEXT_CONTROL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_CONTROL").ValueAsString()}}, 
                {WOW64_CONTEXT_INTEGER, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_INTEGER").ValueAsString()}}, 
                {WOW64_CONTEXT_SEGMENTS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_SEGMENTS").ValueAsString()}}, 
                {WOW64_CONTEXT_FLOATING_POINT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_FLOATING_POINT").ValueAsString()}}, 
                {WOW64_CONTEXT_DEBUG_REGISTERS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_DEBUG_REGISTERS").ValueAsString()}}, 
                {WOW64_CONTEXT_EXTENDED_REGISTERS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_EXTENDED_REGISTERS").ValueAsString()}}, 
                {WOW64_CONTEXT_FULL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_FULL").ValueAsString()}}, 
                {WOW64_CONTEXT_ALL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_ALL").ValueAsString()}}, 
                {WOW64_CONTEXT_XSTATE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_XSTATE").ValueAsString()}}, 
                {WOW64_CONTEXT_EXCEPTION_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_EXCEPTION_ACTIVE").ValueAsString()}}, 
                {WOW64_CONTEXT_SERVICE_ACTIVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_SERVICE_ACTIVE").ValueAsString()}}, 
                {WOW64_CONTEXT_EXCEPTION_REQUEST, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_EXCEPTION_REQUEST").ValueAsString()}}, 
                {WOW64_CONTEXT_EXCEPTION_REPORTING, std::wstring{rm.MainResourceMap().GetValue(L"Resources/WOW64_CONTEXT_EXCEPTION_REPORTING").ValueAsString()}}, 
            };

            return wow64_thread_context_flag_masks;
        }

        std::map<uint32_t, std::wstring> const& mx_csr_register_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            // ReSharper disable CommentTypo
            // https://help.totalview.io/previous_releases/2024.2/HTML/index.html#page/TotalView/Intelx86MXSCRRegister.html
            // ReSharper restore CommentTypo
            static std::map<uint32_t, std::wstring> const mx_csr_register_masks =
            {
                {0x8000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_FZ").ValueAsString()}}, 
                {0x0000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_RC_RN").ValueAsString()}}, 
                {0x2000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_RC_RP").ValueAsString()}}, 
                {0x4000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_RC_RM").ValueAsString()}}, 
                {0x6000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_RC_RZ").ValueAsString()}}, 
                {0x1000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_PM").ValueAsString()}}, 
                {0x0800, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_UM").ValueAsString()}}, 
                {0x0400, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_OM").ValueAsString()}}, 
                {0x0200, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_ZM").ValueAsString()}}, 
                {0x0100, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_DM").ValueAsString()}}, 
                {0x0080, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EM_IM").ValueAsString()}}, 
                {0x0040, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_DAZ").ValueAsString()}}, 
                {0x0020, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_PE").ValueAsString()}}, 
                {0x0010, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_UE").ValueAsString()}}, 
                {0x0008, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_OE").ValueAsString()}}, 
                {0x0004, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_ZE").ValueAsString()}}, 
                {0x0002, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_DE").ValueAsString()}}, 
                {0x0001, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MX_CSR_EF_IE").ValueAsString()}}, 
            };

            return mx_csr_register_masks;
        }

        std::map<flags_string_utils::option_mask<uint64_t>, std::wstring> const& flags_register_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            // https://en.wikipedia.org/wiki/FLAGS_register
            static std::map<flags_string_utils::option_mask<uint64_t>, std::wstring> const flags_register_masks =
            {
                {{.mask= 0x0001, .value= 0x0001}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_CF_CY").ValueAsString()}},
                {{.mask= 0x0001, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_CF_NC").ValueAsString()}},
                {{.mask= 0x0002, .value= std::nullopt }, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_Reserved").ValueAsString()}},
                {{.mask= 0x0004, .value= 0x0004}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_PF_PE").ValueAsString()}},
                {{.mask= 0x0004, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_PF_PO").ValueAsString()}},
                {{.mask= 0x0008, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_Reserved").ValueAsString()}},
                {{.mask= 0x0010, .value= 0x0010}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_AF_AC").ValueAsString()}},
                {{.mask= 0x0010, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_AF_NA").ValueAsString()}},
                {{.mask= 0x0020, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_Reserved").ValueAsString()}},
                {{.mask= 0x0040, .value= 0x0040}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_ZF_ZR").ValueAsString()}},
                {{.mask= 0x0040, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_ZF_NZ").ValueAsString()}},
                {{.mask= 0x0080, .value= 0x0080}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_SF_NG").ValueAsString()}},
                {{.mask= 0x0080, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_SF_PL").ValueAsString()}},
                {{.mask= 0x0100, .value= 0x0100}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_TF").ValueAsString()}},
                {{.mask= 0x0200, .value= 0x0200}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_IF_EI").ValueAsString()}},
                {{.mask= 0x0200, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_IF_DI").ValueAsString()}},
                {{.mask= 0x0400, .value= 0x0400}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_DF_DN").ValueAsString()}},
                {{.mask= 0x0400, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_DF_UP").ValueAsString()}},
                {{.mask= 0x0800, .value= 0x0800}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_OF_OV").ValueAsString()}},
                {{.mask= 0x0800, .value= 0x0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_OF_NV").ValueAsString()}},
                {{.mask= 0x3000, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_IOPL").ValueAsString()}}, 
                {{.mask= 0x4000, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_NT").ValueAsString()}}, 
                {{.mask= 0x0002'0000, .value= 0x0002'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_VM").ValueAsString()}}, 
                {{.mask= 0x0004'0000, .value= 0x0004'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_AC").ValueAsString()}}, 
                {{.mask= 0x0008'0000, .value= 0x0008'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_VIF").ValueAsString()}}, 
                {{.mask= 0x0010'0000, .value= 0x0010'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_VIP").ValueAsString()}}, 
                {{.mask= 0x0020'0000, .value= 0x0020'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_CPUID").ValueAsString()}}, 
                {{.mask= 0x4000'0000, .value= 0x4000'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_AES").ValueAsString()}}, 
                {{.mask= 0x8000'0000, .value= 0x8000'0000}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/EFLAGS_AISE").ValueAsString()}}, 
            };

            return flags_register_masks;
        }

        std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const& npx_status_word_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf

            static std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_status_word_masks =
            {
                {{.mask= 0x0001, .value= static_cast<unsigned short>(0x0001)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_IE").ValueAsString()}},
                {{.mask= 0x0002, .value= static_cast<unsigned short>(0x0002)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_DE").ValueAsString()}},
                {{.mask= 0x0004, .value= static_cast<unsigned short>(0x0004)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_ZE").ValueAsString()}},
                {{.mask= 0x0008, .value= static_cast<unsigned short>(0x0008)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_OE").ValueAsString()}},
                {{.mask= 0x0010, .value= static_cast<unsigned short>(0x0010)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_UE").ValueAsString()}},
                {{.mask= 0x0020, .value= static_cast<unsigned short>(0x0020)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_PE").ValueAsString()}},
                {{.mask= 0x0040, .value= static_cast<unsigned short>(0x0040)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_SF").ValueAsString()}},
                {{.mask= 0x0080, .value= static_cast<unsigned short>(0x0080)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_ES").ValueAsString()}},
                {{.mask= 0x0240, .value= static_cast<unsigned short>(0x0040)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_SU").ValueAsString()}},
                {{.mask= 0x0240, .value= static_cast<unsigned short>(0x0240)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_SO").ValueAsString()}},
                {{.mask= 0x0100, .value= static_cast<unsigned short>(0x0000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C0_0").ValueAsString()}},
                {{.mask= 0x0100, .value= static_cast<unsigned short>(0x0100)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C0_1").ValueAsString()}},
                {{.mask= 0x0200, .value= static_cast<unsigned short>(0x0000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C1_0").ValueAsString()}},
                {{.mask= 0x0200, .value= static_cast<unsigned short>(0x0200)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C1_1").ValueAsString()}},
                {{.mask= 0x0400, .value= static_cast<unsigned short>(0x0000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C2_0").ValueAsString()}},
                {{.mask= 0x0400, .value= static_cast<unsigned short>(0x0400)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C2_1").ValueAsString()}},
                {{.mask= 0x4000, .value= static_cast<unsigned short>(0x0000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C3_0").ValueAsString()}},
                {{.mask= 0x4000, .value= static_cast<unsigned short>(0x4000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_C3_1").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(0 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST0").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(1 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST1").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(2 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST2").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(3 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST3").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(4 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST4").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(5 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST5").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(6 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST6").ValueAsString()}},
                {{.mask= 0x3800, .value= static_cast<unsigned short>(7 << 11)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_TOP_ST7").ValueAsString()}},
                {{.mask= 0x8000, .value= static_cast<unsigned short>(0x8000)},  std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_STATUS_BUSY").ValueAsString()}},
            };
            return npx_status_word_masks;
        }

        std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const& npx_control_word_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf
            static std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_control_word_masks =
            {
                {{.mask= 0x0001, .value= static_cast<unsigned short>(0x0001)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_IM").ValueAsString()}},
                {{.mask= 0x0002, .value= static_cast<unsigned short>(0x0002)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_DM").ValueAsString()}},
                {{.mask= 0x0004, .value= static_cast<unsigned short>(0x0004)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_ZM").ValueAsString()}},
                {{.mask= 0x0008, .value= static_cast<unsigned short>(0x0008)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_OM").ValueAsString()}},
                {{.mask= 0x0010, .value= static_cast<unsigned short>(0x0010)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_UM").ValueAsString()}},
                {{.mask= 0x0020, .value= static_cast<unsigned short>(0x0020)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_PM").ValueAsString()}},
                {{.mask= 0x00C0, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_reserved").ValueAsString()}}, 
                {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0000)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_24BIT").ValueAsString()}}, 
                {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0100)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_reserved_precision").ValueAsString()}}, 
                {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0200)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_53BIT").ValueAsString()}}, 
                {{.mask= 0x0300, .value= static_cast<unsigned short>(0x0300)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_64BIT").ValueAsString()}}, 
                {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0000)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_round_to_nearest").ValueAsString()}}, 
                {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0400)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_round_down").ValueAsString()}}, 
                {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0800)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_round_up").ValueAsString()}}, 
                {{.mask= 0x0C00, .value= static_cast<unsigned short>(0x0C00)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_chop").ValueAsString()}}, 
                {{.mask= 0x1000, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_infinity_control").ValueAsString()}}, 
                {{.mask= 0xE000, .value= std::nullopt}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_CONTROL_reserved").ValueAsString()}}, 
            };
            return npx_control_word_masks;
        }

        std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const& npx_tag_word_masks_map()
        {
            winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

            // http://www.bitsavers.org/components/intel/80386/231917-001_80387_Programmers_Reference_Manual_1987.pdf
            static std::map<flags_string_utils::option_mask<uint16_t>, std::wstring> const npx_tag_word_masks =
            {
                {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0000 <<  0)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST0_valid").ValueAsString()}},
                {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0001 <<  0)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST0_zero").ValueAsString()}},
                {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0002 <<  0)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST0_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 0, .value= static_cast<unsigned short>(0x0003 <<  0)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST0_empty").ValueAsString()}},
                {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0000 <<  1)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST1_valid").ValueAsString()}},
                {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0001 <<  1)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST1_zero").ValueAsString()}},
                {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0002 <<  1)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST1_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 1, .value= static_cast<unsigned short>(0x0003 <<  1)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST1_empty").ValueAsString()}},
                {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0000 <<  2)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST2_valid").ValueAsString()}},
                {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0001 <<  2)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST2_zero").ValueAsString()}},
                {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0002 <<  2)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST2_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 2, .value= static_cast<unsigned short>(0x0003 <<  2)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST2_empty").ValueAsString()}},
                {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0000 <<  3)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST3_valid").ValueAsString()}},
                {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0001 <<  3)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST3_zero").ValueAsString()}},
                {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0002 <<  3)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST3_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 3, .value= static_cast<unsigned short>(0x0003 <<  3)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST3_empty").ValueAsString()}},
                {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0000 <<  4)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST4_valid").ValueAsString()}},
                {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0001 <<  4)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST4_zero").ValueAsString()}},
                {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0002 <<  4)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST4_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 4, .value= static_cast<unsigned short>(0x0003 <<  4)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST4_empty").ValueAsString()}},
                {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0000 <<  5)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST5_valid").ValueAsString()}},
                {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0001 <<  5)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST5_zero").ValueAsString()}},
                {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0002 <<  5)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST5_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 5, .value= static_cast<unsigned short>(0x0003 <<  5)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST5_empty").ValueAsString()}},
                {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0000 <<  6)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST6_valid").ValueAsString()}},
                {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0001 <<  6)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST6_zero").ValueAsString()}},
                {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0002 <<  6)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST6_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 6, .value= static_cast<unsigned short>(0x0003 <<  6)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST6_empty").ValueAsString()}},
                {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0000 <<  7)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST7_valid").ValueAsString()}},
                {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0001 <<  7)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST7_zero").ValueAsString()}},
                {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0002 <<  7)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST7_invalid").ValueAsString()}},
                {{.mask= 0x0003 << 7, .value= static_cast<unsigned short>(0x0003 <<  7)}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NPX_ST7_empty").ValueAsString()}},
            };
            return npx_tag_word_masks;
        }
    }

    std::wstring get_x86_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, x86_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_x64_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, x64_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_wow64_thread_context_flags_to_string(uint32_t const context_flags)
    {
        return generate_flags_string(context_flags, wow64_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_mx_csr_register_to_string(uint32_t const mx_csr)
    {
        return generate_flags_string(mx_csr, mx_csr_register_masks_map(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring get_flags_register_to_string(uint64_t const flags)
    {
        return generate_flags_string(flags, flags_register_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_status_word_to_string(uint16_t const status_word)
    {
        return generate_flags_string(status_word, npx_status_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_control_word_to_string(uint16_t const control_word)
    {
        return generate_flags_string(control_word, npx_control_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::wstring get_npx_tag_word_to_string(uint16_t const tag_word)
    {
        return generate_flags_string(tag_word, npx_tag_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_x86_thread_context_flags(uint32_t const context_flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(context_flags, x86_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_x64_thread_context_flags(uint32_t const context_flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(context_flags, x64_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_wow64_thread_context_flags(uint32_t const context_flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(context_flags, wow64_thread_context_flag_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_mx_csr_register(uint32_t const mx_csr)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(mx_csr, mx_csr_register_masks_map(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> get_flags_register(uint64_t const flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(flags, flags_register_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_status_word(uint16_t const status_word)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(status_word, npx_status_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_control_word(uint16_t const control_word)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(control_word, npx_control_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }

    std::vector<std::wstring> get_npx_tag_word(uint16_t const tag_word)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(tag_word, npx_tag_word_masks_map(), flags_string_utils::mask_used_flags_t{false});
    }
}