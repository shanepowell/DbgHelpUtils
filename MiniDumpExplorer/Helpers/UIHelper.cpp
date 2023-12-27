#include "pch.h"
#include "UIHelper.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/MiniDumpExplorer.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>

#include "ControlHelpers.h"
#include "Utility/InspectableUtility.h"

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Automation::Peers;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::MiniDumpExplorer;

namespace
{
    Windows::Foundation::IInspectable GetFocusedElement(UIElement const& uiElement)
    {
        if (ControlHelpers::IsXamlRootAvailable() && uiElement.XamlRoot())
        {
            return Input::FocusManager::GetFocusedElement(uiElement.XamlRoot());
        }

        return Input::FocusManager::GetFocusedElement();
    }
}

void UIHelper::AnnounceActionForAccessibility(UIElement const& ue, hstring const& announcement, hstring const& activityID)
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

void UIHelper::EnsureMeasured(UIElement const& element)
{
    if (element.DesiredSize().Height == 0)
    {
        element.Measure(Windows::Foundation::Size{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
    }
}

void UIHelper::SetStyleWithType(FrameworkElement const& element, Style const& style)
{
    if (element.Style() != style && (!style || !style.TargetType().Name.empty()))
    {
        element.Style(style);
    }
}

Windows::Foundation::Point UIHelper::Translate(UIElement const& fromElement, UIElement const& toElement, Windows::Foundation::Point const& fromPoint)
{
    if (fromElement == toElement)
    {
        return fromPoint;
    }

    return fromElement.TransformToVisual(toElement).TransformPoint(fromPoint);
}

bool UIHelper::ContainsChild(DependencyObject const& element, DependencyObject child)
{
    if (!element)
    {
        return false;
    }

    while (child)
    {
        if (child == element)
        {
            return true;
        }

        // Walk up the visual tree.  If the root is hit, try using the framework element's
        // parent.  This is done because Popups behave differently with respect to the visual tree,
        // and it could have a parent even if the VisualTreeHelper doesn't find it.
        auto parent = Media::VisualTreeHelper::GetParent(child);
        if (!parent)
        {
            
            if (auto const childElement = child.try_as<FrameworkElement>();
                childElement)
            {
                parent = childElement.Parent();
            }
        }

        child = parent;
    }

    return false;
}

bool UIHelper::ContainsFocusedElement(DependencyObject const& element, UIElement const& uiElement)
{
    return (!element) ? false : ContainsChild(element, GetFocusedElement(uiElement).try_as<DependencyObject>());
}

hstring UIHelper::ObjectToString(Windows::Foundation::IInspectable const& object)
{
    if (auto const displayValue = object.try_as<hstring>(); displayValue)
    {
        return displayValue.value();
    }

    if (auto const displayName = object.try_as<Windows::Foundation::IStringable>(); displayName)
    {
        return displayName.ToString();
    }

    std::wstringstream ss;
    if(InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t, float, double, bool>(
        [&ss](auto const v)
        {
            ss << std::to_wstring(v);
            return true;
        }
        , object
        , false))
    {
        return hstring{ss.str()};
    }

    // fallback
    return get_class_name(object);
}

