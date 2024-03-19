#pragma once
#include "App.xaml.h"

using namespace winrt;
using namespace std::string_view_literals;

class ConverterTools
{
public:
    static bool TryParseBool(Windows::Foundation::IInspectable const& parameter);
    static Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType);
};
