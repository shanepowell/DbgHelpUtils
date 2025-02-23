#include "pch.h"
#include "HandleOperationListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/handle_operation_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/HandleOperationStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"
#include "Utility/run.h"

#if __has_include("HandleOperationListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleOperationListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::HandleOperationStreamEntry const& a, MiniDumpExplorer::HandleOperationStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::Index)},
                {L"Handle", Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::Handle)},
                {L"ProcessId", Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::ProcessId)},
                {L"ThreadId", Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::ThreadId)},
                {L"OperationType", Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::OperationTypeString)},
                {L"BackTraceInformation.Index", [](auto const& a, auto const& b)
                    {
                        return Utility::SortCompare(a.BackTraceInformation().Index(), b.BackTraceInformation().Index());
                    }
                },
                {L"BackTraceInformation.Depth", [](auto const& a, auto const& b)
                    {
                        return Utility::SortCompare(a.BackTraceInformation().Depth(), b.BackTraceInformation().Depth());
                    }
                },
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::HandleOperationStreamEntry::Index)}
            };

    }

    HandleOperationListStreamEntriesDataSource::HandleOperationListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool HandleOperationListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool HandleOperationListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool HandleOperationListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool HandleOperationListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool HandleOperationListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool HandleOperationListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool HandleOperationListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool HandleOperationListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool HandleOperationListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring HandleOperationListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::HandleOperationStreamEntry>().Name;
    }

    hstring HandleOperationListStreamEntriesDataSource::DataTypeName()
    {
        return L"HandleOperationStreamEntry";
    }

    bool HandleOperationListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView HandleOperationListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> HandleOperationListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool HandleOperationListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void HandleOperationListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void HandleOperationListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring HandleOperationListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void HandleOperationListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget HandleOperationListStreamEntriesDataSource::LoadMiniDumpHandleOperationStream(dlg_help_utils::handle_operation_list_stream operation_list_stream)
    {
        auto index = operation_list_stream.index();
        co_await Utility::run(__FUNCTION__, [this, operation_list_stream = std::move(operation_list_stream)]()->Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                entries_.Clear();

                auto weak_self = get_weak();
                co_await resume_background();

                for (size_t index = 0; auto const* operation : operation_list_stream.list())
                {
                    if(WindowHelper::IsExiting())
                    {
                        co_return;
                    }

                    MiniDumpExplorer::HandleOperationStreamEntry entry;
                    entry.as<HandleOperationStreamEntry>()->Set(static_cast<uint32_t>(index), operation);

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
    void HandleOperationListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
