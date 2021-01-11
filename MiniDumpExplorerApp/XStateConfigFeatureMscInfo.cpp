#include "pch.h"
#include "XStateConfigFeatureMscInfo.h"

#include "GetResult.h"
#include "XStateFeature.h"

using namespace std::string_view_literals;

namespace MiniDumpExplorerApp
{
    XStateConfigFeatureMscInfo::XStateConfigFeatureMscInfo(XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate_data)
        : xstate_data_{xstate_data}
    {
        if(HasXStateLegacyFloatingPoint())
        {
            xstate_legacy_floating_point_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_LEGACY_FLOATING_POINT], L"XSTATE_LEGACY_FLOATING_POINT"sv);
        }
        if(HasXStateLegacySSE())
        {
            xstate_legacy_sse_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_LEGACY_SSE], L"XSTATE_LEGACY_SSE"sv);
        }
        if(HasXStateAvx())
        {
            xstate_avx_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_AVX], L"XSTATE_AVX"sv);
        }
        if(HasXStateMpxBndregs())
        {
            xstate_mpx_bndregs_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_MPX_BNDREGS], L"XSTATE_MPX_BNDREGS"sv);
        }
        if(HasXStateMpxBndcsr())
        {
            xstate_mpx_bndcsr_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_MPX_BNDCSR], L"XSTATE_MPX_BNDCSR"sv);
        }
        if(HasXStateAvx512KMask())
        {
            xstate_avx512_kmask_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_AVX512_KMASK], L"XSTATE_AVX512_KMASK"sv);
        }
        if(HasXStateAvx512ZmmH())
        {
            xstate_avx512_zmm_h_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_AVX512_ZMM_H], L"XSTATE_AVX512_ZMM_H"sv);
        }
        if(HasXStateAvx512Zmm())
        {
            xstate_avx512_zmm_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_AVX512_ZMM], L"XSTATE_AVX512_ZMM"sv);
        }
        if(HasXStateIpt())
        {
            xstate_ipt_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_IPT], L"XSTATE_IPT"sv);
        }
        if(HasXStateCetU())
        {
            xstate_cet_u_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_CET_U], L"XSTATE_CET_U"sv);
        }
        if(HasXStateLwp())
        {
            xstate_lwp_ = *winrt::make_self<XStateFeature>(xstate_data.Features[XSTATE_LWP], L"XSTATE_LWP"sv);
        }
    }

    uint32_t XStateConfigFeatureMscInfo::ContextSize() const
    {
        return xstate_data_.ContextSize;
    }

    uint64_t XStateConfigFeatureMscInfo::EnabledFeatures() const
    {
        return xstate_data_.EnabledFeatures;
    }

    bool XStateConfigFeatureMscInfo::HasXStateLegacyFloatingPoint() const
    {
        return IsFeatureEnabled(XSTATE_LEGACY_FLOATING_POINT);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateLegacyFloatingPoint() const
    {
        return xstate_legacy_floating_point_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateLegacySSE() const
    {
        return IsFeatureEnabled(XSTATE_LEGACY_SSE);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateLegacySSE() const
    {
        return xstate_legacy_sse_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateAvx() const
    {
        return IsFeatureEnabled(XSTATE_AVX);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateAvx() const
    {
        return xstate_avx_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateMpxBndregs() const
    {
        return IsFeatureEnabled(XSTATE_MPX_BNDREGS);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateMpxBndregs() const
    {
        return xstate_mpx_bndregs_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateMpxBndcsr() const
    {
        return IsFeatureEnabled(XSTATE_MPX_BNDCSR);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateMpxBndcsr() const
    {
        return xstate_mpx_bndcsr_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateAvx512KMask() const
    {
        return IsFeatureEnabled(XSTATE_AVX512_KMASK);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateAvx512KMask() const
    {
        return xstate_avx512_kmask_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateAvx512ZmmH() const
    {
        return IsFeatureEnabled(XSTATE_AVX512_ZMM_H);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateAvx512ZmmH() const
    {
        return xstate_avx512_zmm_h_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateAvx512Zmm() const
    {
        return IsFeatureEnabled(XSTATE_AVX512_ZMM);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateAvx512Zmm() const
    {
        return xstate_avx512_zmm_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateIpt() const
    {
        return IsFeatureEnabled(XSTATE_IPT);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateIpt() const
    {
        return xstate_ipt_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateCetU() const
    {
        return IsFeatureEnabled(XSTATE_CET_U);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateCetU() const
    {
        return xstate_cet_u_;
    }

    bool XStateConfigFeatureMscInfo::HasXStateLwp() const
    {
        return IsFeatureEnabled(XSTATE_LWP);
    }

    winrt::MiniDumpExplorer::IXStateFeature XStateConfigFeatureMscInfo::XStateLwp() const
    {
        return xstate_lwp_;
    }
}
