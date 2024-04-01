#pragma once

#include "DirectoryStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpLocationDescriptor;

    struct DirectoryStreamEntry : DirectoryStreamEntryT<DirectoryStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<DirectoryStreamEntry>
    {
        DirectoryStreamEntry();

        void Set(uint32_t index, MINIDUMP_DIRECTORY const* stream_entry);

        uint32_t Index() const { return index_; }
        uint32_t StreamType() const;
        hstring StreamTypeName() const { return stream_type_name_; }
        MiniDumpExplorer::MiniDumpLocationDescriptor Location() const { return location_; }
        std::wstring StreamTypeTag() const;

    private:
        uint32_t index_{};
        MINIDUMP_DIRECTORY const* stream_entry_{nullptr};
        MiniDumpExplorer::MiniDumpLocationDescriptor location_{};
        hstring stream_type_name_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DirectoryStreamEntry : DirectoryStreamEntryT<DirectoryStreamEntry, implementation::DirectoryStreamEntry>
    {
    };
}
