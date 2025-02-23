#include "pch.h"
#include "MemoryListStreamEntriesDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/memory_list_stream.h"
#include "Helpers/WindowHelper.h"
#include "Models/MemoryListStreamEntry.h"
#include "Utility/DataGridColumnSorter.h"
#include "Utility/run.h"

#if __has_include("MemoryListStreamEntriesDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryListStreamEntriesDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        std::unordered_map<std::wstring, std::function<Utility::CompareResult(MiniDumpExplorer::MemoryListStreamEntry const& a, MiniDumpExplorer::MemoryListStreamEntry const& b)>> const ColumnSorters
            {
                {L"Index", Utility::MakeComparer(&MiniDumpExplorer::MemoryListStreamEntry::Index)},
                {L"StartOfMemoryRange", Utility::MakeComparer(&MiniDumpExplorer::MemoryListStreamEntry::StartOfMemoryRange)},
                {L"EndOfMemoryRange", Utility::MakeComparer(&MiniDumpExplorer::MemoryListStreamEntry::EndOfMemoryRange)},
                {L"MemorySize", [](MiniDumpExplorer::MemoryListStreamEntry const& a, MiniDumpExplorer::MemoryListStreamEntry const& b)
                    {
                        return Utility::SortCompare(a.Memory().DataSize(), b.Memory().DataSize());
                    }
                },
                {Utility::UnorderedTag, Utility::MakeComparer(&MiniDumpExplorer::MemoryListStreamEntry::Index)}
            };

    }

    MemoryListStreamEntriesDataSource::MemoryListStreamEntriesDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(entries_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool MemoryListStreamEntriesDataSource::IsReadOnly() const
    {
        return true;
    }

    bool MemoryListStreamEntriesDataSource::CanSort() const
    {
        return true;
    }

    bool MemoryListStreamEntriesDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool MemoryListStreamEntriesDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool MemoryListStreamEntriesDataSource::IsAddingNew() const
    {
        return false;
    }

    bool MemoryListStreamEntriesDataSource::CanGroup() const
    {
        return false;
    }

    bool MemoryListStreamEntriesDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool MemoryListStreamEntriesDataSource::HasDataProperties() const
    {
        return true;
    }

    bool MemoryListStreamEntriesDataSource::HasDataType() const
    {
        return true;
    }

    hstring MemoryListStreamEntriesDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::MemoryListStreamEntry>().Name;
    }

    hstring MemoryListStreamEntriesDataSource::DataTypeName()
    {
        return L"MemoryListStreamEntry";
    }

    bool MemoryListStreamEntriesDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView MemoryListStreamEntriesDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> MemoryListStreamEntriesDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool MemoryListStreamEntriesDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void MemoryListStreamEntriesDataSource::ClearDataProperties()
    {
    }

    void MemoryListStreamEntriesDataSource::UpdateDataProperties()
    {
    }

    hstring MemoryListStreamEntriesDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    void MemoryListStreamEntriesDataSource::Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        ColumnSort(entries_, ColumnSorters, dataGrid, args);
    }

    fire_and_forget MemoryListStreamEntriesDataSource::LoadMiniDumpMemoryStream(dlg_help_utils::memory_list_stream memory_list)
    {
// ReSharper disable once IdentifierTypo
// #define C4789_REPRO
#if defined(C4789_REPRO)
        // the following code generates the following error when compiled in x64 Release:
        // ReSharper disable CommentTypo
        // C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Tools\MSVC\14.44.34823\include\__msvc_ranges_tuple_formatter.hpp(625): error C4789: in function 'struct winrt::fire_and_forget __cdecl winrt::MiniDumpExplorer::implementation::MemoryListStreamEntriesDataSource::LoadMiniDumpMemoryStream$_ResumeCoro$1(class dlg_help_utils::memory_list_stream) __ptr64' buffer '' of size 16 bytes will be overrun; 8 bytes will be written starting at offset 16
        // ReSharper restore CommentTypo
        try
        {
            // ReSharper disable once CppTooWideScope
            apartment_context ui_thread;

            entries_.Clear();

            auto weak_self = get_weak();
            co_await resume_background();

            for (size_t index = 0; auto const* memory_range : memory_list.list())
            {
                if(WindowHelper::IsExiting())
                {
                    co_return;
                }

                MiniDumpExplorer::MemoryListStreamEntry entry;
                entry.as<MemoryListStreamEntry>()->Set(static_cast<uint32_t>(index), memory_range);

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
        catch (dlg_help_utils::exceptions::wide_runtime_error const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("LoadMiniDumpMemoryStream (wre) failed for stream index: {0}: {1}\n", memory_list.index(), dlg_help_utils::string_conversation::wstring_to_utf8(e.message())));
        }
        catch (std::runtime_error const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("LoadMiniDumpMemoryStream (re) failed for stream index: {0}: {1}\n", memory_list.index(), e.what()));
        }
        catch (std::exception const& e)
        {
            logger::Log().LogMessage(log_level::error, std::format("LoadMiniDumpMemoryStream (e) failed for stream index: {0}: {1}\n", memory_list.index(), e.what()));
        }
        catch (...)
        {
            logger::Log().LogMessage(log_level::error, std::format("LoadMiniDumpMemoryStream (u) failed for stream index: {0}\n", memory_list.index()));
        }
#else
        auto index = memory_list.index();
        co_await Utility::run(__FUNCTION__, [this, memory_list = std::move(memory_list)]()->Windows::Foundation::IAsyncAction
            {
                // ReSharper disable once CppTooWideScope
                apartment_context ui_thread;

                entries_.Clear();

                auto weak_self = get_weak();
                co_await resume_background();

                for (size_t index = 0; auto const* memory_range : memory_list.list())
                {
                    if(WindowHelper::IsExiting())
                    {
                        co_return;
                    }

                    MiniDumpExplorer::MemoryListStreamEntry entry;
                    entry.as<MemoryListStreamEntry>()->Set(static_cast<uint32_t>(index), memory_range);

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
#endif
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void MemoryListStreamEntriesDataSource::SetupDataProperties()
    {
    }
}
