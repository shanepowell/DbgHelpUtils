#pragma once

#include "MiniDumpModuleProxy.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpModuleProxy : MiniDumpModuleProxyT<MiniDumpModuleProxy>
    {
        MiniDumpModuleProxy(IMiniDumpModule inner);

        [[nodiscard]] hstring Name() const;
        [[nodiscard]] uint64_t BaseOfImage() const;
        [[nodiscard]] uint32_t SizeOfImage() const;
        [[nodiscard]] uint32_t CheckSum() const;
        [[nodiscard]] clock::time_point TimeDateStamp() const;
        [[nodiscard]] IVsFixedFileInfo VersionInfo() const;
        [[nodiscard]] uint32_t CvRecordSize() const;
        [[nodiscard]] bool HasCvRecordHexDump() const;
        [[nodiscard]] hstring CvRecordHexDump() const;
        [[nodiscard]] ICvInfoPdb70 PdbInfo() const;
        [[nodiscard]] uint32_t MiscRecordSize() const;
        [[nodiscard]] bool HasMiscRecordHexDump() const;
        [[nodiscard]] hstring MiscRecordHexDump() const;
        [[nodiscard]] uint64_t Reserved0() const;
        [[nodiscard]] uint64_t Reserved1() const;

        [[nodiscard]] event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token);

    private:
        IMiniDumpModule inner_;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> property_changed_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpModuleProxy : MiniDumpModuleProxyT<MiniDumpModuleProxy, implementation::MiniDumpModuleProxy>
    {
    };
}
