#include "pch.h"
#include "ThreadListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/thread_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("ThreadListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::ThreadListStreamEntry const& a, MiniDumpExplorer::ThreadListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::Index)},
                {L"Id", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::Id)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::Name)},
                {L"Priority", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::Priority)},
                {L"PriorityClass", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::PriorityClass)},
                {L"SuspendCount", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::SuspendCount)},
                {L"TEB", Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::TEB)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::ThreadListStreamEntry::Index)}
            };

    }

    ThreadListStreamEntriesDataSource::ThreadListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ThreadListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool ThreadListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ThreadListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ThreadListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ThreadListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool ThreadListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ThreadListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ThreadListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring ThreadListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::ThreadListStreamEntry>().Name;
    }

    hstring ThreadListStreamEntriesDataSource::DataTypeName()
    {
        return L"ThreadListStreamEntry";
    }

    bool ThreadListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ThreadListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ThreadListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ThreadListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void ThreadListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring ThreadListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ThreadListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget ThreadListStreamEntriesDataSource::LoadMiniDumpThreadStream(dlg_help_utils::thread_list_stream const thread_list)
    {
        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        entries_.Clear();

        auto weak_self = get_weak();
        co_await resume_background();

        for (size_t index = 0; auto const& thread : thread_list.list())
        {
            if(WindowHelper::IsExiting())
            {
                co_return;
            }

            MiniDumpExplorer::ThreadListStreamEntry entry;
            entry.as<ThreadListStreamEntry>()->Set(static_cast<uint32_t>(index), std::move(thread));

            if(WindowHelper::IsExiting())
            {
                co_return;
            }

            co_await ui_thread;

            if(auto const self = weak_self.get();
                self && !WindowHelper::IsExiting())
            {
                entries_.Append(entry);
            }
            else
            {
                // it's been removed while loading the items
                co_return;
            }

            co_await resume_background();

            ++index;
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ThreadListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
