#pragma once

class UIHelper
{
public:
    static void AnnounceActionForAccessibility(winrt::Microsoft::UI::Xaml::UIElement const& ue, winrt::hstring const& announcement, winrt::hstring const& activityID);
    static void CopyToClipboard(winrt::hstring const& text);
};
