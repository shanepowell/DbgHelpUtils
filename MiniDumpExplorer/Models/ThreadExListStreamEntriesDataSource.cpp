#include "pch.h"
#include "ThreadExListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/run.h"
#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadExListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("ThreadExListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadExListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::ThreadExListStreamEntry const& a, MiniDumpExplorer::ThreadExListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::Index)},
                {L"Id", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::Id)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::Name)},
                {L"Priority", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::Priority)},
                {L"PriorityClass", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::PriorityClass)},
                {L"SuspendCount", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::SuspendCount)},
                {L"TEB", Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::TEB)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::ThreadExListStreamEntry::Index)}
            };

    }

    ThreadExListStreamEntriesDataSource::ThreadExListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadExListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ThreadExListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool ThreadExListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ThreadExListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ThreadExListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ThreadExListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool ThreadExListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ThreadExListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ThreadExListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring ThreadExListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::ThreadExListStreamEntry>().Name;
    }

    hstring ThreadExListStreamEntriesDataSource::DataTypeName()
    {
        return L"ThreadExListStreamEntry";
    }

    bool ThreadExListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ThreadExListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ThreadExListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadExListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ThreadExListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void ThreadExListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring ThreadExListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ThreadExListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget ThreadExListStreamEntriesDataSource::LoadMiniDumpThreadStream(dlg_help_utils::thread_ex_list_stream thread_list)
    {
        auto index = thread_list.index();
        co_await Utility::run(__FUNCTION__, [this, thread_list = std::move(thread_list)]() -> Windows::Foundation::IAsyncAction
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

                    MiniDumpExplorer::ThreadExListStreamEntry entry;
                    entry.as<ThreadExListStreamEntry>()->Set(static_cast<uint32_t>(index), thread);

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
            }, [index] { return Utility::for_stream_index(index); });
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ThreadExListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
