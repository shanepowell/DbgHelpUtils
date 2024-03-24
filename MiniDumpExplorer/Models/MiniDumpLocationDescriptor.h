#pragma once

#include "MiniDumpLocationDescriptor.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpLocationDescriptor : MiniDumpLocationDescriptorT<MiniDumpLocationDescriptor>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpLocationDescriptor>
    {
        MiniDumpLocationDescriptor();

        void Set(MINIDUMP_LOCATION_DESCRIPTOR const& locationDescriptor);

        uint32_t DataSize() const { return locationDescriptor_.DataSize; }
        uint32_t Rva() const { return locationDescriptor_.Rva; }

    private:
        MINIDUMP_LOCATION_DESCRIPTOR locationDescriptor_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpLocationDescriptor : MiniDumpLocationDescriptorT<MiniDumpLocationDescriptor, implementation::MiniDumpLocationDescriptor>
    {
    };
}
