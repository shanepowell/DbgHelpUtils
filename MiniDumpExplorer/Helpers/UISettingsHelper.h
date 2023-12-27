#pragma once

class UISettingsHelper
{
public:
    static bool AreSettingsEnablingAnimations();
    static bool AreSettingsAutoHidingScrollBars();

private:
    static void EnsureSettings();

private:
    static winrt::Windows::UI::ViewManagement::UISettings uiSettings_;
};
