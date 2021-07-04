#include "pch.h"
#include "MiniDumpModuleProxy.h"

#include <winrt/impl/MiniDumpExplorer.1.h>
#if __has_include("MiniDumpModuleProxy.g.cpp")
#include "MiniDumpModuleProxy.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpModuleProxy::MiniDumpModuleProxy(IMiniDumpModule inner)
        : inner_{inner}
    {
    }

    hstring MiniDumpModuleProxy::Name() const
    {
        return inner_.Name();
    }

    uint64_t MiniDumpModuleProxy::BaseOfImage() const
    {
        return inner_.BaseOfImage();
    }

    uint32_t MiniDumpModuleProxy::SizeOfImage() const
    {
        return inner_.SizeOfImage();
    }

    uint32_t MiniDumpModuleProxy::CheckSum() const
    {
        return inner_.CheckSum();
    }

    clock::time_point MiniDumpModuleProxy::TimeDateStamp() const
    {
        return inner_.TimeDateStamp();
    }

    IVsFixedFileInfo MiniDumpModuleProxy::VersionInfo() const
    {
        return inner_.VersionInfo();
    }

    uint32_t MiniDumpModuleProxy::CvRecordSize() const
    {
        return inner_.CvRecordSize();
    }

    bool MiniDumpModuleProxy::HasCvRecordHexDump() const
    {
        return inner_.HasCvRecordHexDump();
    }

    hstring MiniDumpModuleProxy::CvRecordHexDump() const
    {
        return inner_.CvRecordHexDump();
    }

    ICvInfoPdb70 MiniDumpModuleProxy::PdbInfo() const
    {
        return inner_.PdbInfo();
    }

    uint32_t MiniDumpModuleProxy::MiscRecordSize() const
    {
        return inner_.MiscRecordSize();
    }

    bool MiniDumpModuleProxy::HasMiscRecordHexDump() const
    {
        return inner_.HasMiscRecordHexDump();
    }

    hstring MiniDumpModuleProxy::MiscRecordHexDump() const
    {
        return inner_.MiscRecordHexDump();
    }

    uint64_t MiniDumpModuleProxy::Reserved0() const
    {
        return inner_.Reserved0();
    }

    uint64_t MiniDumpModuleProxy::Reserved1() const
    {
        return inner_.Reserved1();
    }

    event_token MiniDumpModuleProxy::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return property_changed_.add(handler);
    }

    void MiniDumpModuleProxy::PropertyChanged(event_token const& token)
    {
        property_changed_.remove(token);
    }

}
