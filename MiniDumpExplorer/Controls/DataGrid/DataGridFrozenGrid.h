#pragma once

#include "DataGridFrozenGrid.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridFrozenGrid : DataGridFrozenGridT<DataGridFrozenGrid>
    {
        DataGridFrozenGrid();

        static Microsoft::UI::Xaml::DependencyProperty IsFrozenProperty();
        static bool GetIsFrozen(Microsoft::UI::Xaml::DependencyObject const& element);
        static void SetIsFrozen(Microsoft::UI::Xaml::DependencyObject const& element, bool value);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridFrozenGrid : DataGridFrozenGridT<DataGridFrozenGrid, implementation::DataGridFrozenGrid>
    {
    };
}
