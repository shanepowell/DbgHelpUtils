#pragma once

#include "DurationConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DurationConverter : DurationConverterT<DurationConverter>
    {
        DurationConverter() = default;

        [[nodiscard]] DurationSourceType Source() const;
        void Source(DurationSourceType value);

        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);

    private:
        DurationSourceType source_{DurationSourceType::Milliseconds};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DurationConverter : DurationConverterT<DurationConverter, implementation::DurationConverter>
    {
    };
}
