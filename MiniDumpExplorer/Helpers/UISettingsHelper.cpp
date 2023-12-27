#include "pch.h"
#include "UISettingsHelper.h"

#include <winrt/Windows.UI.ViewManagement.h>

using namespace winrt::Windows::UI::ViewManagement;

UISettings UISettingsHelper::uiSettings_{nullptr};

bool UISettingsHelper::AreSettingsEnablingAnimations()
{
    EnsureSettings();
    return uiSettings_.AnimationsEnabled();
}

bool UISettingsHelper::AreSettingsAutoHidingScrollBars()
{
    EnsureSettings();
    return uiSettings_.AutoHideScrollBars();
}

void UISettingsHelper::EnsureSettings()
{
    if(!uiSettings_)
    {
        uiSettings_ = UISettings{};
    }
}
