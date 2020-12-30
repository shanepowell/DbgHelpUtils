#include "pch.h"
#include "BooleanToVisibilityConverter.h"
#if __has_include("BooleanToVisibilityConverter.g.cpp")
#include "BooleanToVisibilityConverter.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    bool BooleanToVisibilityConverter::IsReversed() const
    {
        return reversed_;
    }

    void BooleanToVisibilityConverter::IsReversed(bool const value)
    {
        reversed_ = value;
    }

    Windows::Foundation::IInspectable BooleanToVisibilityConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        auto val = unbox_value_or(value, false);
        if(reversed_)
        {
            val = !val;
        }

        if(val)
        {
            return box_value(Windows::UI::Xaml::Visibility::Visible);
        }

        return box_value(Windows::UI::Xaml::Visibility::Collapsed);
    }

    Windows::Foundation::IInspectable BooleanToVisibilityConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
