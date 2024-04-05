#pragma once

#include "MiniDumpLocationDescriptor64.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpLocationDescriptor64 : MiniDumpLocationDescriptor64T<MiniDumpLocationDescriptor64>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpLocationDescriptor64>
    {
        MiniDumpLocationDescriptor64();

        void Set(MINIDUMP_LOCATION_DESCRIPTOR64 const& locationDescriptor);

        uint64_t DataSize() const { return locationDescriptor_.DataSize; }
        uint64_t Rva() const { return locationDescriptor_.Rva; }

    private:
        MINIDUMP_LOCATION_DESCRIPTOR64 locationDescriptor_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpLocationDescriptor64 : MiniDumpLocationDescriptor64T<MiniDumpLocationDescriptor64, implementation::MiniDumpLocationDescriptor64>
    {
    };
}
