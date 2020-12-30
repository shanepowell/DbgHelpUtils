#include "pch.h"
#include "DateTimeConverter.h"

#include "../DbgHelpUtils/time_utils.h"

#if __has_include("DateTimeConverter.g.cpp")
#include "DateTimeConverter.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    bool DateTimeConverter::IsUtc() const
    {
        return utc_;
    }

    void DateTimeConverter::IsUtc(bool const value)
    {
        utc_ = value;
    }

    Windows::Foundation::IInspectable DateTimeConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language) const
    {
        auto const val = unbox_value<clock::time_point>(value);
        auto const str = utc_ ? dlg_help_utils::time_utils::to_utc_time(clock::to_time_t(val)) : dlg_help_utils::time_utils::to_local_time(clock::to_time_t(val));
        return box_value(hstring{str});
    }

    Windows::Foundation::IInspectable DateTimeConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& type_name, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring language)
    {
        throw hresult_not_implemented();
    }
}
