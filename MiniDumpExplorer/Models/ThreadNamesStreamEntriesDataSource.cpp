#include "pch.h"
#include "ThreadNamesStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/thread_names_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadNameListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("ThreadNamesStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadNamesStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::ThreadNameListStreamEntry const& a, MiniDumpExplorer::ThreadNameListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::ThreadNameListStreamEntry::Index)},
                {L"Id", Utility::MakeComparer(&MiniDumpExplorer::ThreadNameListStreamEntry::Id)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::ThreadNameListStreamEntry::Name)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::ThreadNameListStreamEntry::Index)}
            };

    }

    ThreadNamesStreamEntriesDataSource::ThreadNamesStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadNamesStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ThreadNamesStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool ThreadNamesStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ThreadNamesStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ThreadNamesStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ThreadNamesStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool ThreadNamesStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ThreadNamesStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ThreadNamesStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring ThreadNamesStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::ThreadNameListStreamEntry>().Name;
    }

    hstring ThreadNamesStreamEntriesDataSource::DataTypeName()
    {
        return L"ThreadNameListStreamEntry";
    }

    bool ThreadNamesStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ThreadNamesStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ThreadNamesStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadNamesStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ThreadNamesStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void ThreadNamesStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring ThreadNamesStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ThreadNamesStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget ThreadNamesStreamEntriesDataSource::LoadMiniDumpThreadStream(dlg_help_utils::thread_names_list_stream const thread_list)
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

            MiniDumpExplorer::ThreadNameListStreamEntry entry;
            entry.as<ThreadNameListStreamEntry>()->Set(static_cast<uint32_t>(index), thread);

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
    void ThreadNamesStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
