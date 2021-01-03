#pragma once

#include "IndexConverter.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct IndexConverter : IndexConverterT<IndexConverter>
    {
        IndexConverter() = default;

        [[nodiscard]] bool IsBracketed() const;
        void IsBracketed(bool value);
        [[nodiscard]] Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language) const;
        [[nodiscard]] static Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& type_name, Windows::Foundation::IInspectable const& parameter, hstring language);

    private:
        bool bracketed_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct IndexConverter : IndexConverterT<IndexConverter, implementation::IndexConverter>
    {
    };
}
