#include "pch.h"
#include "DirectoryStreamEntry.h"

#include "MiniDumpLocationDescriptor.h"
#include "DbgHelpUtils/mini_dump_stream_type.h"

#if __has_include("DirectoryStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DirectoryStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DirectoryStreamEntry::DirectoryStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"StreamType"
            },
            {
            },
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void DirectoryStreamEntry::Set(uint32_t const index, MINIDUMP_DIRECTORY const* stream_entry)
    {
        index_ = index;
        stream_entry_ = stream_entry;
        stream_type_name_ = dlg_help_utils::mini_dump_stream_type::to_wstring(static_cast<MINIDUMP_STREAM_TYPE>(stream_entry_->StreamType));
        location_.as<MiniDumpLocationDescriptor>()->Set(stream_entry_->Location);
    }

    uint32_t DirectoryStreamEntry::StreamType() const
    {
        if(stream_entry_ == nullptr)
        {
            return std::numeric_limits<uint32_t>::max();
        }

        return stream_entry_->StreamType;
    }

    std::wstring DirectoryStreamEntry::StreamTypeTag() const
    {
        if(stream_entry_ == nullptr)
        {
            return {};
        }

        return dlg_help_utils::mini_dump_stream_type::to_enum_wstring(static_cast<MINIDUMP_STREAM_TYPE>(stream_entry_->StreamType));
    }
}
