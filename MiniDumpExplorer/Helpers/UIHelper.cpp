#include "pch.h"
#include "UIHelper.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml::Automation::Peers;
using namespace winrt::Windows::ApplicationModel;

void UIHelper::AnnounceActionForAccessibility(Microsoft::UI::Xaml::UIElement const& ue, hstring const& announcement, hstring const& activityID)
{
    auto const peer = FrameworkElementAutomationPeer::FromElement(ue);
    peer.RaiseNotificationEvent(AutomationNotificationKind::ActionCompleted
                                , AutomationNotificationProcessing::ImportantMostRecent
                                , announcement
                                , activityID);
}

void UIHelper::CopyToClipboard(hstring const& text)
{
    DataTransfer::DataPackage const dataPackage;
    dataPackage.SetText(text);
    DataTransfer::Clipboard::SetContent(dataPackage);
}
