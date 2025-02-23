#include "pch.h"
#include "FunctionTableStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/function_table_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/FunctionStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"
#include "Utility/run.h"

#if __has_include("FunctionTableStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "FunctionTableStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::FunctionStreamEntry const& a, MiniDumpExplorer::FunctionStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::Index)},
                {L"MinimumAddress", Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::MinimumAddress)},
                {L"MaximumAddress", Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::MaximumAddress)},
                {L"BaseAddress", Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::BaseAddress)},
                {L"EntryCount", Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::EntryCount)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::FunctionStreamEntry::Index)}
            };

    }

    FunctionTableStreamEntriesDataSource::FunctionTableStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool FunctionTableStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool FunctionTableStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool FunctionTableStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool FunctionTableStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool FunctionTableStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool FunctionTableStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool FunctionTableStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool FunctionTableStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool FunctionTableStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring FunctionTableStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::FunctionStreamEntry>().Name;
    }

    hstring FunctionTableStreamEntriesDataSource::DataTypeName()
    {
        return L"FunctionStreamEntry";
    }

    bool FunctionTableStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView FunctionTableStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> FunctionTableStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool FunctionTableStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void FunctionTableStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void FunctionTableStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring FunctionTableStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void FunctionTableStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }
    fire_and_forget FunctionTableStreamEntriesDataSource::LoadMiniDumpFunctionTableStream(dlg_help_utils::function_table_stream function_table)
    {
        auto index = function_table.index();
        co_await Utility::run(__FUNCTION__, [this, function_table = std::move(function_table)]() -> Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                entries_.Clear();

                auto weak_self = get_weak();
                co_await resume_background();

                for (size_t index = 0; auto const& function_stream : function_table.list())
                {
                    if(WindowHelper::IsExiting())
                    {
                        co_return;
                    }

                    MiniDumpExplorer::FunctionStreamEntry entry;
                    entry.as<FunctionStreamEntry>()->Set(static_cast<uint32_t>(index), function_stream);

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
    void FunctionTableStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
