#include "pch.h"
#include "DumpFileView.h"
#if __has_include("DumpFileView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DumpFileView.g.cpp"
#endif

#include <DbgHelp.h>

#include "Category.h"
/*
#include "../DbgHelpUtils/wide_runtime_error.h"
#include "../DbgHelpUtils/mini_dump_stream_type.h"
*/

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace dlg_help_utils;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpFileView::DumpFileView(hstring path, hstring display_name)
        : display_name_{ std::move(display_name) }
        , path_{ std::move(path) }
        /*, dump_file_{ std::wstring{path_} }*/
    {
        InitializeComponent();

        categories_ = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();

        /*
        try
        {
            dump_file_.open_mini_dump();
        }
        catch ([[maybe_unused]] exceptions::wide_runtime_error const& e)
        {
            SetupInvalidUserModeDump();
            return;
        }

        if (dump_file_.header() == nullptr)
        {
            SetupInvalidUserModeDump();
            return;
        }

        switch (dump_file_.type())
        {
        case dump_file_type::user_mode_dump:
            SetupUserModeDump();
            break;

        case dump_file_type::invalid:
            SetupInvalidUserModeDump();
            break;

        case dump_file_type::kernel_mode_dump_x86:
            SetupX86KernelMemoryDump();
            break;

        case dump_file_type::kernel_mode_dump_x64:
            SetupX64KernelMemoryDump();
            break;
        }
    */
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
        /*
        if (args.IsSettingsInvoked())
        {
            // Navigate to Settings.
        }
        else if (args.InvokedItemContainer())
        {
            Interop::TypeName page_type_name;
            page_type_name.Name = unbox_value<hstring>(args.InvokedItemContainer().Tag());
            page_type_name.Kind = Interop::TypeKind::Primitive;
            ContentFrame().Navigate(page_type_name, nullptr);
        }
    */
    }

    [[noreturn]] void DumpFileView::ContentFrame_NavigationFailed([[maybe_unused]] Windows::Foundation::IInspectable const& sender,
                                                     Navigation::NavigationFailedEventArgs const& args)
    {
        throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + args.SourcePageType().Name);
    }

    void DumpFileView::SetupInvalidUserModeDump()
    {
    }

    void DumpFileView::SetupUserModeDump()
    {
        /*
        auto const* header_data = dump_file_.header();
        if (header_data == nullptr)
        {
            return;
        }

        const auto header_menu = winrt::make_self<Category>(L"Header", L"Icon", nullptr, true);
        categories_.Append(*header_menu);

        auto const* directory = dump_file_.directory();
        if (directory == nullptr)
        {
            return;
        }

        auto stream_types = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        for (size_t index = 0; index < header_data->NumberOfStreams; ++index)
        {
            auto const& entry = directory[index];

            std::wostringstream ss;
            ss << L"Stream: [" << index << "] " << mini_dump_stream_type::to_string(static_cast<MINIDUMP_STREAM_TYPE>(entry.StreamType));

            const auto stream_menu = winrt::make_self<Category>(ss.str(), L"Icon", nullptr, true);
            stream_types.Append(*stream_menu);
        }

        const auto streams_menu = winrt::make_self<Category>(L"Streams", L"Icon", stream_types);
        categories_.Append(*streams_menu);
    */
    }

    void DumpFileView::SetupX86KernelMemoryDump()
    {
    }

    void DumpFileView::SetupX64KernelMemoryDump()
    {
    }
}
