#include "pch.h"
#include "TokenStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/run.h"
#include "DbgHelpUtils/token_info_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/TokenStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"

#if __has_include("TokenStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "TokenStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::TokenStreamEntry const& a, MiniDumpExplorer::TokenStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::TokenStreamEntry::Index)},
                {L"TokenId", Utility::MakeComparer(&MiniDumpExplorer::TokenStreamEntry::TokenId)},
                {L"TokenHandle", Utility::MakeComparer(&MiniDumpExplorer::TokenStreamEntry::TokenHandle)},
                {L"TokenDataSize", Utility::MakeComparer(&MiniDumpExplorer::TokenStreamEntry::TokenDataSize)},
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::TokenStreamEntry::Index)}
            };
    }

    TokenStreamEntriesDataSource::TokenStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool TokenStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool TokenStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool TokenStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool TokenStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool TokenStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool TokenStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool TokenStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool TokenStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool TokenStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring TokenStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::TokenStreamEntry>().Name;
    }

    hstring TokenStreamEntriesDataSource::DataTypeName()
    {
        return L"TokenStreamEntry";
    }

    bool TokenStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView TokenStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> TokenStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool TokenStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void TokenStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void TokenStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring TokenStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void TokenStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget TokenStreamEntriesDataSource::LoadMiniDumpTokenStream(dlg_help_utils::token_info_list_stream token_stream)
    {
        auto index = token_stream.index();
        co_await Utility::run(__FUNCTION__, [this, token_stream = std::move(token_stream)]()->Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                entries_.Clear();

                auto weak_self = get_weak();
                co_await resume_background();

                for (size_t index = 0; auto const& token : token_stream.list())
                {
                    if(WindowHelper::IsExiting())
                    {
                        co_return;
                    }

                    MiniDumpExplorer::TokenStreamEntry entry;
                    entry.as<TokenStreamEntry>()->Set(static_cast<uint32_t>(index), token);

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
    void TokenStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
