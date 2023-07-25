#include "pch.h"
#include "RecentFileItem.h"
#if __has_include("RecentFileItem.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentFileItem.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    RecentFileItem::RecentFileItem()
    {
    }

    hstring RecentFileItem::Name()
    {
        return L"";
    }

    Media::Imaging::BitmapSource RecentFileItem::Icon()
    {
        return nullptr;
    }

    hstring RecentFileItem::Location()
    {
        return L"";
    }

    hstring RecentFileItem::Size()
    {
        return L"";
    }
}
