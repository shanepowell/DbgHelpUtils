#include "pch.h"
#include "MemoryInfoListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/memory_info_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/MemoryInfoListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("MemoryInfoListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryInfoListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::MemoryInfoListStreamEntry const& a, MiniDumpExplorer::MemoryInfoListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::Index)},
                {L"BaseAddress", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::BaseAddress)},
                {L"AllocationBase", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::AllocationBase)},
                {L"AllocationProtectListString", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::AllocationProtectListString)},
                {L"RegionSize", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::RegionSize)},
                {L"StateString", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::StateString)},
                {L"ProtectListString", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::ProtectListString)},
                {L"TypeString", Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::TypeString)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::MemoryInfoListStreamEntry::Index)}
            };

    }

    MemoryInfoListStreamEntriesDataSource::MemoryInfoListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool MemoryInfoListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool MemoryInfoListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool MemoryInfoListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool MemoryInfoListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool MemoryInfoListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool MemoryInfoListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool MemoryInfoListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool MemoryInfoListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool MemoryInfoListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring MemoryInfoListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::MemoryInfoListStreamEntry>().Name;
    }

    hstring MemoryInfoListStreamEntriesDataSource::DataTypeName()
    {
        return L"MemoryInfoListStreamEntry";
    }

    bool MemoryInfoListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView MemoryInfoListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> MemoryInfoListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool MemoryInfoListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void MemoryInfoListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void MemoryInfoListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring MemoryInfoListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void MemoryInfoListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget MemoryInfoListStreamEntriesDataSource::LoadMiniDumpMemoryStream(dlg_help_utils::memory_info_list_stream const memory_list)
    {
        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        entries_.Clear();

        auto weak_self = get_weak();
        co_await resume_background();

        for (size_t index = 0; auto const memory_range : memory_list.list())
        {
            if(WindowHelper::IsExiting())
            {
                co_return;
            }

            MiniDumpExplorer::MemoryInfoListStreamEntry entry;
            entry.as<MemoryInfoListStreamEntry>()->Set(static_cast<uint32_t>(index), memory_range);

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
    void MemoryInfoListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
