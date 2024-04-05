#include "pch.h"
#include "Memory64ListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/memory64_list_stream.h"
#include "Models/Memory64ListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("Memory64ListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Memory64ListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::Memory64ListStreamEntry const& a, MiniDumpExplorer::Memory64ListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::Memory64ListStreamEntry::Index)},
                {L"StartOfMemoryRange", Utility::MakeComparer(&MiniDumpExplorer::Memory64ListStreamEntry::StartOfMemoryRange)},
                {L"EndOfMemoryRange", Utility::MakeComparer(&MiniDumpExplorer::Memory64ListStreamEntry::EndOfMemoryRange)},
                {L"MemorySize", [](MiniDumpExplorer::Memory64ListStreamEntry const& a, MiniDumpExplorer::Memory64ListStreamEntry const& b)
                    {
                        return Utility::SortCompare(a.Location().DataSize(), b.Location().DataSize());
                    }
                },
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::Memory64ListStreamEntry::Index)}
            };

    }

    Memory64ListStreamEntriesDataSource::Memory64ListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool Memory64ListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool Memory64ListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool Memory64ListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool Memory64ListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool Memory64ListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool Memory64ListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool Memory64ListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool Memory64ListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool Memory64ListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring Memory64ListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::Memory64ListStreamEntry>().Name;
    }

    hstring Memory64ListStreamEntriesDataSource::DataTypeName()
    {
        return L"Memory64ListStreamEntry";
    }

    bool Memory64ListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView Memory64ListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> Memory64ListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool Memory64ListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void Memory64ListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void Memory64ListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring Memory64ListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void Memory64ListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    void Memory64ListStreamEntriesDataSource::LoadMiniDumpMemoryStream(dlg_help_utils::memory64_list_stream const& memory_list) const
    {
        entries_.Clear();

        for (size_t index = 0; auto const memory_range : memory_list.list())
        {
            MiniDumpExplorer::Memory64ListStreamEntry entry;
            entry.as<Memory64ListStreamEntry>()->Set(static_cast<uint32_t>(index), memory_range);
            entries_.Append(entry);
            ++index;
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void Memory64ListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
