#include "pch.h"
#include "DumpFileView.h"
#if __has_include("DumpFileView.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DumpFileView.g.cpp"
#endif

#include "Category.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpFileView::DumpFileView(hstring path, hstring display_name)
        : display_name_{std::move(display_name)}
          , path_{std::move(path)}
    {
        InitializeComponent();

        categories_ = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();

        const auto menuItem10 = winrt::make_self<Category>(L"Menu item 10", L"Icon", nullptr, true);

        const auto menuItem9 = winrt::make_self<Category>(L"Menu item 9", L"Icon", nullptr, true);
        const auto menuItem8 = winrt::make_self<Category>(L"Menu item 8", L"Icon", nullptr, true);
        auto menuItem7Children = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        menuItem7Children.Append(*menuItem9);
        menuItem7Children.Append(*menuItem8);

        const auto menuItem7 = winrt::make_self<Category>(L"Menu item 7", L"Icon", menuItem7Children);
        auto menuItem6Children = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        menuItem6Children.Append(*menuItem7);

        const auto menuItem6 = winrt::make_self<Category>(L"Menu item 6", L"Icon", menuItem6Children);

        const auto menuItem5 = winrt::make_self<Category>(L"Menu item 5", L"Icon", nullptr, true);
        const auto menuItem4 = winrt::make_self<Category>(L"Menu item 4", L"Icon", nullptr, true);
        auto menuItem3Children = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        menuItem3Children.Append(*menuItem5);
        menuItem3Children.Append(*menuItem4);

        const auto menuItem3 = winrt::make_self<Category>(L"Menu item 3", L"Icon", menuItem3Children);
        auto menuItem2Children = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        menuItem2Children.Append(*menuItem3);

        const auto menuItem2 = winrt::make_self<Category>(L"Menu item 2", L"Icon", menuItem2Children);
        auto menuItem1Children = winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>();
        menuItem1Children.Append(*menuItem2);

        const auto menuItem1 = winrt::make_self<Category>(L"Menu item 1", L"Icon", menuItem1Children);

        categories_.Append(*menuItem1);
        categories_.Append(*menuItem6);
        categories_.Append(*menuItem10);
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
}
