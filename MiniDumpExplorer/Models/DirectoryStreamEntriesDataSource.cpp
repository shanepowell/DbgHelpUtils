#include "pch.h"
#include "DirectoryStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Models/DirectoryStreamEntry.h"

#if __has_include("DirectoryStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DirectoryStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DirectoryStreamEntriesDataSource::DirectoryStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool DirectoryStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool DirectoryStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool DirectoryStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool DirectoryStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool DirectoryStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool DirectoryStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool DirectoryStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool DirectoryStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool DirectoryStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring DirectoryStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::DirectoryStreamEntry>().Name;
    }

    hstring DirectoryStreamEntriesDataSource::DataTypeName()
    {
        return L"DirectoryStreamEntry";
    }

    bool DirectoryStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView DirectoryStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> DirectoryStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool DirectoryStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void DirectoryStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void DirectoryStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring DirectoryStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void DirectoryStreamEntriesDataSource::Sort([[maybe_unused]] MiniDumpExplorer::DataGrid const& dataGrid, [[maybe_unused]] MiniDumpExplorer::DataGridColumnEventArgs const& args)
    {
    }

    void DirectoryStreamEntriesDataSource::LoadMiniDumpStreams(dlg_help_utils::mini_dump const& mini_dump) const
    {
        entries_.Clear();
        auto const* header = mini_dump.header();
        auto const* directory = mini_dump.directory();

        if(directory == nullptr)
        {
            return;
        }

        for (size_t index = 0; index < header->NumberOfStreams; ++index)
        {
            auto const& stream_entry = directory[index];
            MiniDumpExplorer::DirectoryStreamEntry entry;
            entry.as<DirectoryStreamEntry>()->Set(static_cast<uint32_t>(index), &stream_entry);
            entries_.Append(entry);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DirectoryStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
