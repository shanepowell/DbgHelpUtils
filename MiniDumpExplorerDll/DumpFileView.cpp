#include "pch.h"
#include "DumpFileView.h"
#if __has_include("DumpFileView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DumpFileView.g.cpp"
#endif

#include "Category.h"
#include "DumpHeaderView.h"
#include "DumpStreamCommentView.h"
#include "DumpStreamExceptionView.h"
#include "DumpStreamFunctionTableView.h"
#include "DumpStreamHandleDataView.h"
#include "DumpStreamHandleOperationListView.h"
#include "DumpStreamIptTraceView.h"
#include "DumpStreamMemory64ListView.h"
#include "DumpStreamMemoryInfoListView.h"
#include "DumpStreamMemoryListView.h"
#include "DumpStreamMiscInfoView.h"
#include "DumpStreamModuleListView.h"
#include "DumpStreamProcessVmCountersView.h"
#include "DumpStreamSystemInfoView.h"
#include "DumpStreamSystemMemoryInfoView.h"
#include "DumpStreamThreadExListView.h"
#include "DumpStreamThreadInfoListView.h"
#include "DumpStreamThreadListView.h"
#include "DumpStreamThreadNamesView.h"
#include "DumpStreamTokenView.h"
#include "DumpStreamUnloadedModuleListView.h"
#include "DumpStreamUnsupportedView.h"
#include "DumpStreamUnusedView.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpFileView::DumpFileView(hstring path, hstring display_name, IDumpFile dump_file)
        : display_name_{ std::move(display_name) }
        , path_{ std::move(path) }
        , dump_file_{ dump_file }
    {
        InitializeComponent();

        switch (dump_file_.Type())
        {
        default:
        case DumpFileType::Invalid:
            SetupInvalidUserModeDump();
            break;
        case DumpFileType::UserModeDump:
            SetupUserModeDump();
            break;
        case DumpFileType::KernelModeDumpX86:
            SetupX86KernelMemoryDump();
            break;
        case DumpFileType::KernelModeDumpX64:
            SetupX64KernelMemoryDump();
            break;
        }
    }

    hstring DumpFileView::DisplayName() const
    {
        return display_name_;
    }

    void DumpFileView::DisplayName(hstring value)
    {
        if (display_name_ != value)
        {
            display_name_ = std::move(value);
            property_changed_(*this, Data::PropertyChangedEventArgs{L"DisplayName"});
        }
    }

    hstring DumpFileView::Path() const
    {
        return path_;
    }

    void DumpFileView::Path(hstring value)
    {
        if (path_ != value)
        {
            path_ = std::move(value);
            property_changed_(*this, Data::PropertyChangedEventArgs{L"Path"});
        }
    }

    Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> DumpFileView::Categories() const
    {
        return categories_;
    }

    event_token DumpFileView::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return property_changed_.add(handler);
    }

    void DumpFileView::PropertyChanged(event_token const& token)
    {
        property_changed_.remove(token);
    }

    void DumpFileView::OnItemInvoked([[maybe_unused]] muxc::NavigationView const& sender,
                                     [[maybe_unused]] muxc::NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.IsSettingsInvoked())
        {
            // Navigate to Settings.
        }
        else if (args.InvokedItemContainer())
        {
            auto const name = std::wstring{winrt::unbox_value_or<hstring>(args.InvokedItemContainer().Tag(), L"")};
            auto const it = pages_.find(name);
            if (it != pages_.end())
            {
                sender.Content(it->second);
            }
        }
    }

    void DumpFileView::SetupInvalidUserModeDump()
    {
        SetupDumpHeaderPage();
    }

    void DumpFileView::SetupUserModeDump()
    {
        SetupDumpHeaderPage();

        if(!dump_file_.HasHeader())
        {
            return;
        }

        // for some reason I don't understand yet, adding more than one sub-cat will not display correctly.
        // auto stream_types = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        for (auto const& stream : dump_file_.Streams())
        {
            std::wostringstream ss;
            ss << L"[" << stream.Index() << "] " << stream.TypeString().c_str();

            const auto stream_menu = winrt::make_self<Category>(hstring{ss.str()}, L"Icon", nullptr, true);
            pages_.emplace(std::make_pair(ss.str(), CreateStreamPage(stream)));
            categories_.Append(*stream_menu);
            // stream_types.Append(*stream_menu);
        }

        /*
        const auto streams_menu = winrt::make_self<Category>(L"Streams", L"Icon", stream_types, false);
        categories_.Append(*streams_menu);
        */
    }

    void DumpFileView::SetupX86KernelMemoryDump()
    {
        SetupDumpHeaderPage();
    }

    void DumpFileView::SetupX64KernelMemoryDump()
    {
        SetupDumpHeaderPage();
    }

    void DumpFileView::SetupDumpHeaderPage()
    {
        const auto header_menu = winrt::make_self<Category>(L"Header", L"Icon", nullptr, true);
        categories_.Append(*header_menu);

        const auto header_page = winrt::make_self<DumpHeaderView>(dump_file_.Header());
        pages_.emplace(std::make_pair(std::wstring{header_menu->Name()}, header_page.as<Controls::UserControl>()));

        NavView().SelectedItem(*header_menu);
        NavView().Content(*header_page);
    }

    Controls::UserControl DumpFileView::CreateStreamPage(IDumpFileStream const& stream) const
    {
        switch(stream.Type())
        {
        case StreamType::UnusedStream:
            return winrt::make_self<DumpStreamUnusedView>().as<Controls::UserControl>();

        case StreamType::ThreadListStream:
            return winrt::make_self<DumpStreamThreadListView>().as<Controls::UserControl>();

        case StreamType::ModuleListStream:
            return winrt::make_self<DumpStreamModuleListView>().as<Controls::UserControl>();

        case StreamType::MemoryListStream:
            return winrt::make_self<DumpStreamMemoryListView>().as<Controls::UserControl>();

        case StreamType::ExceptionStream:
            return winrt::make_self<DumpStreamExceptionView>(stream.AsException()).as<Controls::UserControl>();

        case StreamType::SystemInfoStream:
            return winrt::make_self<DumpStreamSystemInfoView>().as<Controls::UserControl>();

        case StreamType::ThreadExListStream:
            return winrt::make_self<DumpStreamThreadExListView>().as<Controls::UserControl>();

        case StreamType::Memory64ListStream:
            return winrt::make_self<DumpStreamMemory64ListView>().as<Controls::UserControl>();

        case StreamType::CommentStreamA:
        case StreamType::CommentStreamW:
            return winrt::make_self<DumpStreamCommentView>(stream.AsComment()).as<Controls::UserControl>();

        case StreamType::HandleDataStream:
            return winrt::make_self<DumpStreamHandleDataView>().as<Controls::UserControl>();

        case StreamType::FunctionTableStream:
            return winrt::make_self<DumpStreamFunctionTableView>().as<Controls::UserControl>();

        case StreamType::UnloadedModuleListStream:
            return winrt::make_self<DumpStreamUnloadedModuleListView>().as<Controls::UserControl>();

        case StreamType::MiscInfoStream:
            return winrt::make_self<DumpStreamMiscInfoView>().as<Controls::UserControl>();

        case StreamType::MemoryInfoListStream:
            return winrt::make_self<DumpStreamMemoryInfoListView>().as<Controls::UserControl>();

        case StreamType::ThreadInfoListStream:
            return winrt::make_self<DumpStreamThreadInfoListView>().as<Controls::UserControl>();

        case StreamType::HandleOperationListStream:
            return winrt::make_self<DumpStreamHandleOperationListView>().as<Controls::UserControl>();

        case StreamType::TokenStream:
            return winrt::make_self<DumpStreamTokenView>().as<Controls::UserControl>();

        case StreamType::SystemMemoryInfoStream:
            return winrt::make_self<DumpStreamSystemMemoryInfoView>().as<Controls::UserControl>();

        case StreamType::ProcessVmCountersStream:
            return winrt::make_self<DumpStreamProcessVmCountersView>().as<Controls::UserControl>();

        case StreamType::IptTraceStream:
            return winrt::make_self<DumpStreamIptTraceView>().as<Controls::UserControl>();

        case StreamType::ThreadNamesStream:
            return winrt::make_self<DumpStreamThreadNamesView>().as<Controls::UserControl>();

        case StreamType::ReservedStream0:
        case StreamType::ReservedStream1:
        case StreamType::JavaScriptDataStream:
        case StreamType::ceStreamNull:
        case StreamType::ceStreamSystemInfo:
        case StreamType::ceStreamException:
        case StreamType::ceStreamModuleList:
        case StreamType::ceStreamProcessList:
        case StreamType::ceStreamThreadList:
        case StreamType::ceStreamThreadContextList:
        case StreamType::ceStreamThreadCallStackList:
        case StreamType::ceStreamMemoryVirtualList:
        case StreamType::ceStreamMemoryPhysicalList:
        case StreamType::ceStreamBucketParameters:
        case StreamType::ceStreamProcessModuleMap:
        case StreamType::ceStreamDiagnosisList:
        case StreamType::LastReservedStream:
        default:
            {
                auto const page = winrt::make_self<DumpStreamUnsupportedView>();
                return page.as<Controls::UserControl>();
            }
        }
    }
}
