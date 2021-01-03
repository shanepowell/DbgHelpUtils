#include "pch.h"
#include "IndexConverter.h"
#if __has_include("IndexConverter.g.cpp")
#include "IndexConverter.g.cpp"
#endif

#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt::MiniDumpExplorer::implementation
{
    bool IndexConverter::IsBracketed() const
    {
        return bracketed_;
    }

    void IndexConverter::IsBracketed(bool const value)
    {
        bracketed_ = value;
    }

    Windows::Foundation::IInspectable IndexConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        auto const presenter = value.as<Windows::UI::Xaml::Controls::Primitives::ListViewItemPresenter>();
        auto const item = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(presenter).as<Windows::UI::Xaml::Controls::ListViewItem>();
        auto const listView = Windows::UI::Xaml::Controls::ItemsControl::ItemsControlFromItemContainer(item);
        auto const index = listView.IndexFromContainer(item) + 1;

        if(!bracketed_)
        {
            return box_value(to_hstring(index));
        }

        std::wstringstream ss;
        ss << "[" << index << "]";
        return box_value(hstring{std::move(ss).str()});
    }

    Windows::Foundation::IInspectable IndexConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
