#include "pch.h"
#include "ModuleListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/module_list_stream.h"
#include "Models/ModuleListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("ModuleListStreamEntriesDataSource.g.cpp")
#include "ModuleListStreamEntriesDataSource.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::ModuleListStreamEntry const& a, MiniDumpExplorer::ModuleListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::Index)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::Name)},
                {L"Path", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::Path)},
                {L"Base", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::Base)},
                {L"CheckSum", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::CheckSum)},
                {L"SizeOfImage", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::SizeOfImage)},
                {L"TimeDateStamp", Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::TimeDateStamp)},
                {L"FileVersion", [](MiniDumpExplorer::ModuleListStreamEntry const& a, MiniDumpExplorer::ModuleListStreamEntry const& b)
                    {
                        return Utility::SortCompare(a.FileInfo().FileVersion(), b.FileInfo().FileVersion());
                    }
                },
                {L"ProductVersion", [](MiniDumpExplorer::ModuleListStreamEntry const& a, MiniDumpExplorer::ModuleListStreamEntry const& b)
                    {
                        return Utility::SortCompare(a.FileInfo().ProductVersion(), b.FileInfo().ProductVersion());
                    }
                },
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::ModuleListStreamEntry::Index)}
            };

    }

    ModuleListStreamEntriesDataSource::ModuleListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ModuleListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool ModuleListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool ModuleListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool ModuleListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool ModuleListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool ModuleListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool ModuleListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool ModuleListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool ModuleListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring ModuleListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::ModuleListStreamEntry>().Name;
    }

    hstring ModuleListStreamEntriesDataSource::DataTypeName()
    {
        return L"ModuleListStreamEntry";
    }

    bool ModuleListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView ModuleListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> ModuleListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool ModuleListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void ModuleListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void ModuleListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring ModuleListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void ModuleListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    void ModuleListStreamEntriesDataSource::LoadMiniDumpModuleStream(dlg_help_utils::module_list_stream const& module_list) const
    {
        entries_.Clear();

        for (size_t index = 0; auto const& module : module_list.list())
        {
            MiniDumpExplorer::ModuleListStreamEntry entry;
            entry.as<ModuleListStreamEntry>()->Set(static_cast<uint32_t>(index), std::move(module));
            entries_.Append(entry);
            ++index;
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ModuleListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
