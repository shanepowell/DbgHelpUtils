#include "pch.h"
#include "DirectoryStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Helpers/WindowHelper.h"
#include "Models/DirectoryStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("DirectoryStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DirectoryStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::DirectoryStreamEntry const& a, MiniDumpExplorer::DirectoryStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::DirectoryStreamEntry::Index)},
                {L"Name", Utility::MakeComparer(&MiniDumpExplorer::DirectoryStreamEntry::StreamTypeName)},
                {L"Location", [](MiniDumpExplorer::DirectoryStreamEntry const& a, MiniDumpExplorer::DirectoryStreamEntry const& b)
                    {
                        return Utility::SortCompare(a.Location().Rva(), b.Location().Rva());
                    }},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::DirectoryStreamEntry::Index)}
            };

    }


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

    void DirectoryStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget DirectoryStreamEntriesDataSource::LoadMiniDumpStreams(dlg_help_utils::mini_dump const& mini_dump)
    {
        // ReSharper disable once CppTooWideScope
        apartment_context ui_thread;

        entries_.Clear();

        auto weak_self = get_weak();
        co_await resume_background();

        auto const* header = mini_dump.header();
        auto const* directory = mini_dump.directory();

        if(directory == nullptr)
        {
            co_return;
        }

        for (size_t index = 0; index < header->NumberOfStreams; ++index)
        {
            if(WindowHelper::IsExiting())
            {
                co_return;
            }

            auto const& stream_entry = directory[index];
            MiniDumpExplorer::DirectoryStreamEntry entry;
            entry.as<DirectoryStreamEntry>()->Set(static_cast<uint32_t>(index), &stream_entry);


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

        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DirectoryStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
