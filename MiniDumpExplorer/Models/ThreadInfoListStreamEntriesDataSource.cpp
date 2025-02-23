#include "pch.h"
#include "ThreadInfoListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/run.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/thread_info_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/ThreadInfoListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("ThreadInfoListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadInfoListStreamEntriesDataSource.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::ThreadInfoListStreamEntry const& a, MiniDumpExplorer::ThreadInfoListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::Index)},
                {L"Id", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::Id)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::Name)},
                {L"DumpFlagsListString", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::DumpFlagsListString)},
                {L"CreateTime", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::CreateTime)},
                {L"ExitTime", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::ExitTime)},
                {L"KernelTime", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::KernelTime)},
                {L"UserTime", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::UserTime)},
                {L"StartAddress", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::StartAddress)},
                {L"Affinity", Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::Affinity)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::ThreadInfoListStreamEntry::Index)}
            };

    }

    ThreadInfoListStreamEntriesDataSource::ThreadInfoListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadInfoListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ThreadInfoListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool ThreadInfoListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ThreadInfoListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ThreadInfoListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ThreadInfoListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool ThreadInfoListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ThreadInfoListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ThreadInfoListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring ThreadInfoListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::ThreadInfoListStreamEntry>().Name;
    }

    hstring ThreadInfoListStreamEntriesDataSource::DataTypeName()
    {
        return L"ThreadInfoListStreamEntry";
    }

    bool ThreadInfoListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ThreadInfoListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ThreadInfoListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ThreadInfoListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ThreadInfoListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void ThreadInfoListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring ThreadInfoListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ThreadInfoListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget ThreadInfoListStreamEntriesDataSource::LoadMiniDumpThreadInfoStream(dlg_help_utils::thread_info_list_stream thread_list, dlg_help_utils::time_utils::locale_timezone_info dump_file_timezone_info)
    {
        auto index = thread_list.index();
        co_await Utility::run(__FUNCTION__, [this, thread_list = std::move(thread_list), dump_file_timezone_info = std::move(dump_file_timezone_info)]() -> Windows::Foundation::IAsyncAction
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

                    MiniDumpExplorer::ThreadInfoListStreamEntry entry;
                    entry.as<ThreadInfoListStreamEntry>()->Set(static_cast<uint32_t>(index), thread, dump_file_timezone_info);

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
    void ThreadInfoListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
