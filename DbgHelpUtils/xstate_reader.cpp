#include "xstate_reader.h"

namespace
{
    // ReSharper disable CppInconsistentNaming
    // ReSharper disable IdentifierTypo
    typedef BOOL(WINAPI* PGETXSTATEFEATURESMASK)(PCONTEXT Context, PDWORD64 FeatureMask);
    PGETXSTATEFEATURESMASK pfnGetXStateFeaturesMask = nullptr;

    typedef PVOID(WINAPI* LOCATEXSTATEFEATURE)(PCONTEXT Context, DWORD FeatureId, PDWORD Length);
    LOCATEXSTATEFEATURE pfnLocateXStateFeature = nullptr;
    // ReSharper restore IdentifierTypo
    // ReSharper restore CppInconsistentNaming

    bool load_xstate_functions()
    {
        if (pfnGetXStateFeaturesMask != nullptr &&
            pfnLocateXStateFeature != nullptr)
        {
            return true;
        }

        HMODULE const hModule = GetModuleHandleW(L"kernel32.dll");
        if (hModule == nullptr)
        {
            return false;
        }
        pfnGetXStateFeaturesMask = reinterpret_cast<PGETXSTATEFEATURESMASK>(GetProcAddress(hModule, "GetXStateFeaturesMask"));  // NOLINT(clang-diagnostic-cast-function-type-strict)
        pfnLocateXStateFeature = reinterpret_cast<LOCATEXSTATEFEATURE>(GetProcAddress(hModule, "LocateXStateFeature"));  // NOLINT(clang-diagnostic-cast-function-type-strict)
        return pfnGetXStateFeaturesMask != nullptr
            && pfnLocateXStateFeature != nullptr;
    }

}
namespace dlg_help_utils
{

    xstate_reader::xstate_reader(stream_thread_context::context_x64 const* context)
        : context_{ context }
        , supported_{ context && (context->ContextFlags & X64_CONTEXT_XSTATE) == X64_CONTEXT_XSTATE }
    {
    }

    xstate_reader::xstate_reader(WOW64_CONTEXT const* context)
        : context_{ context }
        , supported_{ context && (context->ContextFlags & WOW64_CONTEXT_XSTATE) == WOW64_CONTEXT_XSTATE }
    {
    }

    bool xstate_reader::is_supported() const
    {
        return supported_;
    }

    bool xstate_reader::is_in_init_state() const
    {
        if (!is_supported())
        {
            return false;
        }

        if (!load_xstate_functions())
        {
            return false;
        }

        DWORD64 featureMask = 0;
        if (!pfnGetXStateFeaturesMask(static_cast<PCONTEXT>(const_cast<void*>(context_)), &featureMask))
        {
            return false;
        }
        return (featureMask & XSTATE_MASK_AVX) == XSTATE_MASK_AVX;
    }

    std::generator<xstate_reader::ymm_register> xstate_reader::ymm_registers() const
    {
        if (!is_supported())
        {
            co_return;
        }
 
        if (!load_xstate_functions())
        {
            co_return;
        }

        DWORD length = 0;
        auto ymm = static_cast<PM128A>(pfnLocateXStateFeature(static_cast<PCONTEXT>(const_cast<void*>(context_)), XSTATE_LEGACY_SSE, &length));
        auto xmm = static_cast<PM128A>(pfnLocateXStateFeature(static_cast<PCONTEXT>(const_cast<void*>(context_)), XSTATE_AVX, nullptr));

        for (uint32_t index = 0; index < length / sizeof(M128A); ++index)
        {
            co_yield ymm_register{ .index = index, .xmm = xmm + index, .ymm = ymm + index };
        }
    }

    struct xstate_reader::ymm_register xstate_reader::get_ymm_register(uint32_t const index) const
    {
        if (!is_supported())
        {
            return {};
        }
        if (!load_xstate_functions())
        {
            return {};
        }
        DWORD length = 0;
        auto ymm = static_cast<PM128A>(pfnLocateXStateFeature(static_cast<PCONTEXT>(const_cast<void*>(context_)), XSTATE_LEGACY_SSE, &length));
        auto xmm = static_cast<PM128A>(pfnLocateXStateFeature(static_cast<PCONTEXT>(const_cast<void*>(context_)), XSTATE_AVX, nullptr));
        return { .index = index, .xmm = xmm + index, .ymm = ymm + index };
    }
}
