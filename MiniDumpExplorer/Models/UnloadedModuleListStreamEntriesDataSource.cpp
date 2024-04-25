#include "pch.h"
#include "UnloadedModuleListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/unloaded_module_list_stream.h"
#include "Models/UnloadedModuleListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("UnloadedModuleListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "UnloadedModuleListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::UnloadedModuleListStreamEntry const& a, MiniDumpExplorer::UnloadedModuleListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::Index)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::Name)},
                {L"Path", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::Path)},
                {L"CheckSum", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::CheckSum)},
                {L"BaseOfImage", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::BaseOfImage)},
                {L"SizeOfImage", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::SizeOfImage)},
                {L"BuildFileHash", Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::BuildFileHash)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::UnloadedModuleListStreamEntry::Index)}
            };

    }

    UnloadedModuleListStreamEntriesDataSource::UnloadedModuleListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool UnloadedModuleListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool UnloadedModuleListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool UnloadedModuleListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool UnloadedModuleListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool UnloadedModuleListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool UnloadedModuleListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool UnloadedModuleListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool UnloadedModuleListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool UnloadedModuleListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring UnloadedModuleListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::UnloadedModuleListStreamEntry>().Name;
    }

    hstring UnloadedModuleListStreamEntriesDataSource::DataTypeName()
    {
        return L"UnloadedModuleListStreamEntry";
    }

    bool UnloadedModuleListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView UnloadedModuleListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> UnloadedModuleListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool UnloadedModuleListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void UnloadedModuleListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void UnloadedModuleListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring UnloadedModuleListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void UnloadedModuleListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    void UnloadedModuleListStreamEntriesDataSource::LoadMiniDumpUnloadedModuleStream(dlg_help_utils::unloaded_module_list_stream const& module_list) const
    {
        entries_.Clear();

        for (size_t index = 0; auto const& module : module_list.list())
        {
            MiniDumpExplorer::UnloadedModuleListStreamEntry entry;
            entry.as<UnloadedModuleListStreamEntry>()->Set(static_cast<uint32_t>(index), std::move(module));
            entries_.Append(entry);
            ++index;
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void UnloadedModuleListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
