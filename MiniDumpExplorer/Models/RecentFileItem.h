#pragma once

#include "RecentFileItem.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem>
    {
        RecentFileItem();

        hstring Name();
        Microsoft::UI::Xaml::Media::Imaging::BitmapSource Icon();
        hstring Location();
        hstring Size();
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentFileItem : RecentFileItemT<RecentFileItem, implementation::RecentFileItem>
    {
    };
}
