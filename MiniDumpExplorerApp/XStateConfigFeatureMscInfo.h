#pragma once

#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    struct XStateConfigFeatureMscInfo : winrt::implements<XStateConfigFeatureMscInfo, winrt::MiniDumpExplorer::IXStateConfigFeatureMscInfo>
    {
        XStateConfigFeatureMscInfo(XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate_data);

        [[nodiscard]] uint32_t ContextSize() const;
        [[nodiscard]] uint64_t EnabledFeatures() const;
        [[nodiscard]] bool HasXStateLegacyFloatingPoint() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateLegacyFloatingPoint() const;
        [[nodiscard]] bool HasXStateLegacySSE() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateLegacySSE() const;
        [[nodiscard]] bool HasXStateAvx() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateAvx() const;
        [[nodiscard]] bool HasXStateMpxBndregs() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateMpxBndregs() const;
        [[nodiscard]] bool HasXStateMpxBndcsr() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateMpxBndcsr() const;
        [[nodiscard]] bool HasXStateAvx512KMask() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateAvx512KMask() const;
        [[nodiscard]] bool HasXStateAvx512ZmmH() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateAvx512ZmmH() const;
        [[nodiscard]] bool HasXStateAvx512Zmm() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateAvx512Zmm() const;
        [[nodiscard]] bool HasXStateIpt() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateIpt() const;
        [[nodiscard]] bool HasXStateCetU() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateCetU() const;
        [[nodiscard]] bool HasXStateLwp() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IXStateFeature XStateLwp() const;

    private:
        [[nodiscard]] bool IsFeatureEnabled(uint64_t const feature) const
        {
            auto const mask = 1ui64 << feature;
            return (xstate_data_.EnabledFeatures & mask) == mask;
        }

    private:
        XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate_data_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_legacy_floating_point_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_legacy_sse_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_avx_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_mpx_bndregs_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_mpx_bndcsr_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_avx512_kmask_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_avx512_zmm_h_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_avx512_zmm_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_ipt_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_cet_u_;
        winrt::MiniDumpExplorer::IXStateFeature xstate_lwp_;
    };
}
