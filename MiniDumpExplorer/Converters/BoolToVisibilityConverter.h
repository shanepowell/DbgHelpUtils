#pragma once

#include "BoolToVisibilityConverter.g.h"

#include "BoolToObjectConverter.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct BoolToVisibilityConverter : BoolToVisibilityConverterT<BoolToVisibilityConverter, BoolToObjectConverter>
    {
        BoolToVisibilityConverter();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct BoolToVisibilityConverter : BoolToVisibilityConverterT<BoolToVisibilityConverter, implementation::BoolToVisibilityConverter>
    {
    };
}
