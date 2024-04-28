#include "pch.h"
#include "HandleDataStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/handle_data_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/HandleDataStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("HandleDataStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleDataStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::HandleDataStreamEntry const& a, MiniDumpExplorer::HandleDataStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::HandleDataStreamEntry::Index)},
                {L"Type", Utility::MakeComparer(&MiniDumpExplorer::HandleDataStreamEntry::Type)},
                {L"TotalHandles", Utility::MakeComparer(&MiniDumpExplorer::HandleDataStreamEntry::TotalHandles)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::HandleDataStreamEntry::Index)}
            };

    }

    HandleDataStreamEntriesDataSource::HandleDataStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool HandleDataStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool HandleDataStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool HandleDataStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool HandleDataStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool HandleDataStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool HandleDataStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool HandleDataStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool HandleDataStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool HandleDataStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring HandleDataStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::HandleDataStreamEntry>().Name;
    }

    hstring HandleDataStreamEntriesDataSource::DataTypeName()
    {
        return L"HandleDataStreamEntry";
    }

    bool HandleDataStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView HandleDataStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> HandleDataStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool HandleDataStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void HandleDataStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void HandleDataStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring HandleDataStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void HandleDataStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget HandleDataStreamEntriesDataSource::LoadMiniDumpHandleDataStream(dlg_help_utils::handle_data_stream const handle_data)
    {
        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        entries_.Clear();

        auto weak_self = get_weak();
        co_await resume_background();

        for (size_t index = 0; auto const& [handle_type, handle_total] : handle_data.handle_type_totals())
        {
            if(WindowHelper::IsExiting())
            {
                co_return;
            }

            MiniDumpExplorer::HandleDataStreamEntry entry;
            entry.as<HandleDataStreamEntry>()->Set(static_cast<uint32_t>(index), handle_data, handle_type, handle_total);

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
    void HandleDataStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
