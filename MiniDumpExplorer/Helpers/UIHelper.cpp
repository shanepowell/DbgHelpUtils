#include "pch.h"
#include "UIHelper.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/MiniDumpExplorer.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>

#include "ControlHelpers.h"
#include "Utility/InspectableUtility.h"
#include "Models/GlobalOptionItems.h"

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

    Controls::ToggleMenuFlyoutItem CreateGlobalOptionsCheckedBoundPropertyItem(GlobalOptionItems const& globalOptions, hstring const& resourceName, hstring const& propertyName)
    {
        auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
        auto const menuItem = Controls::ToggleMenuFlyoutItem{};
        menuItem.Text(rm.MainResourceMap().GetValue(resourceName).ValueAsString());

        {
            Data::Binding binding{};
            binding.Mode(Data::BindingMode::TwoWay);
            binding.Path(PropertyPath{propertyName});
            binding.Source(globalOptions);
            menuItem.SetBinding(Controls::ToggleMenuFlyoutItem::IsCheckedProperty(), binding);
        }

        return menuItem;
    }

    Controls::RadioMenuFlyoutItem CreateGlobalOptionsGroupCheckedBoundPropertyItem(GlobalOptionItems const& globalOptions, hstring const& resourceName, hstring const& propertyName, hstring const& groupName)
    {
        auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
        auto const menuItem = Controls::RadioMenuFlyoutItem{};
        menuItem.Text(rm.MainResourceMap().GetValue(resourceName).ValueAsString());
        menuItem.GroupName(groupName);

        {
            Data::Binding binding{};
            binding.Mode(Data::BindingMode::TwoWay);
            binding.Path(PropertyPath{propertyName});
            binding.Source(globalOptions);
            menuItem.SetBinding(Controls::RadioMenuFlyoutItem::IsCheckedProperty(), binding);
        }

        return menuItem;
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
        return hstring{displayValue.value()};
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
        return hstring{std::move(ss).str()};
    }

    // fallback
    return get_class_name(object);
}

Controls::MenuFlyoutItem UIHelper::CreateCopyValue(Controls::TextBlock const& control)
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const& menuItem = Controls::MenuFlyoutItem{};
    const Controls::SymbolIcon iconSource;
    iconSource.Symbol(Controls::Symbol::Copy);
    menuItem.Icon(iconSource);
    menuItem.Text(rm.MainResourceMap().GetValue(L"Resources/CopyValueMenuItem").ValueAsString());
    menuItem.Click([control](auto const&, auto const&)
    {
        auto text = control.SelectedText();
        if (text.empty())
        {
            text = control.Text();
        }
        CopyToClipboard(text);
    });

    return menuItem;
}

Controls::MenuFlyoutItem UIHelper::CreateHexNumberDisplayFormatOption()
{
    GlobalOptionItems optionItems{};
    return CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/HexNumberDisplayFormatOptionMenuItem", implementation::GlobalOptionItems::DisplayHexadecimalNumericFormatString);
}

Controls::MenuFlyoutSubItem UIHelper::CreateSizeNumberDisplayUnitFormatOption()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/SizeNumberDisplayUnitFormatMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    hstring groupName{L"SizeNumberDisplayUnitOptions"};

    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatAutoMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatAutoString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatBytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatBytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatKilobytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatKilobytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatMegabytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatMegabytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatGigabytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatGigabytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatTerabytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatTerabytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatPetabytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatPetabytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatExabytesMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatExabytesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayUnitFormatRawMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayUnitFormatRawString, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateSizeNumberDisplayPrintFormatOption()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/SizeNumberDisplayPrintFormatMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    hstring groupName{L"SizeNumberDisplayPrintFormatOptions"};

    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayPrintFormatFullMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayPrintFormatFullString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayPrintFormatCompactMenuItem", implementation::GlobalOptionItems::SizeNumberDisplayPrintFormatCompactString, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateSizeNumberDisplayBaseOption()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/SizeNumberDisplayBaseMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    hstring groupName{L"SizeNumberDisplayBaseOptions"};

    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayBase10MenuItem", implementation::GlobalOptionItems::SizeNumberDisplayBase10String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/SizeNumberDisplayBase16MenuItem", implementation::GlobalOptionItems::SizeNumberDisplayBase16String, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateTimeStampLocale()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/TimeStampLocaleMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    hstring groupName{L"TimeStampLocaleOptions"};

    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/TimeStampLocaleLocalMenuItem", implementation::GlobalOptionItems::TimeStampLocaleLocalString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/TimeStampLocaleUtcMenuItem", implementation::GlobalOptionItems::TimeStampLocaleUtcString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/TimeStampLocaleDumpFileMenuItem", implementation::GlobalOptionItems::TimeStampLocaleDumpFileString, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateDateFormatFlags()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/DateFormatFlagsMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    {
        hstring groupName1{L"DateFormatFlagsOptions1"};
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatLongDateMenuItem", implementation::GlobalOptionItems::DateFormatLongDateString, groupName1));
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatShortDateMenuItem", implementation::GlobalOptionItems::DateFormatShortDateString, groupName1));
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatYearMonthMenuItem", implementation::GlobalOptionItems::DateFormatYearMonthString, groupName1));
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatMonthDayMenuItem", implementation::GlobalOptionItems::DateFormatMonthDayString, groupName1));
    }

    menu.Items().Append(Controls::MenuFlyoutSeparator{});

    {
        hstring groupName2{L"DateFormatFlagsOptions2"};
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatLeftToRightReadingMenuItem", implementation::GlobalOptionItems::DateFormatLeftToRightReadingString, groupName2));
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatRightToLeftReadingMenuItem", implementation::GlobalOptionItems::DateFormatRightToLeftReadingString, groupName2));
        menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatAutoLayoutMenuItem", implementation::GlobalOptionItems::DateFormatAutoLayoutString, groupName2));
    }

    menu.Items().Append(Controls::MenuFlyoutSeparator{});
    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/DateFormatUseAltCalendarMenuItem", implementation::GlobalOptionItems::DateFormatUseAltCalendarString));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateTimeFormatFlags()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/TimeFormatFlagsMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/TimeFormat24HourMenuItem", implementation::GlobalOptionItems::TimeFormat24HourString));
    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/TimeFormatNoTimeMarkerMenuItem", implementation::GlobalOptionItems::TimeFormatNoTimeMarkerString));
    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/TimeFormatNoSecondsMenuItem", implementation::GlobalOptionItems::TimeFormatNoSecondsString));
    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/TimeFormatNoMinutesOrSecondsMenuItem", implementation::GlobalOptionItems::TimeFormatNoMinutesOrSecondsString));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateDurationFormat()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/DurationFormatMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    hstring groupName{L"DurationFormatOptions"};

    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatTimeSpanMenuItem", implementation::GlobalOptionItems::DurationFormatTimeSpanString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatAutoMenuItem", implementation::GlobalOptionItems::DurationFormatAutoString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatFileTimeMenuItem", implementation::GlobalOptionItems::DurationFormatFileTimeString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatMillisecondsMenuItem", implementation::GlobalOptionItems::DurationFormatMillisecondsString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatSecondsMenuItem", implementation::GlobalOptionItems::DurationFormatSecondsString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatMinutesMenuItem", implementation::GlobalOptionItems::DurationFormatMinutesString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatHoursMenuItem", implementation::GlobalOptionItems::DurationFormatHoursString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatDaysMenuItem", implementation::GlobalOptionItems::DurationFormatDaysString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatWeeksMenuItem", implementation::GlobalOptionItems::DurationFormatWeeksString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatMonthsMenuItem", implementation::GlobalOptionItems::DurationFormatMonthsString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatYearsMenuItem", implementation::GlobalOptionItems::DurationFormatYearsString, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/DurationFormatRawMenuItem", implementation::GlobalOptionItems::DurationFormatRawString, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateM128AOption()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};
    menu.Text(rm.MainResourceMap().GetValue(L"Resources/M128AOptionMenu").ValueAsString());
    GlobalOptionItems optionItems{};
    hstring groupName{ L"M128AOptions" };
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionInt128MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatInt128String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionUInt128MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatUInt128String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionInt64MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatInt64String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionUInt64MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatUInt64String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionInt32MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatInt32String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionUInt32MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatUInt32String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionInt16MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatInt16String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionUInt16MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatUInt16String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionInt8MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatInt8String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionUInt8MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatUInt8String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionFloat32MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatFloat32String, groupName));
    menu.Items().Append(CreateGlobalOptionsGroupCheckedBoundPropertyItem(optionItems, L"Resources/M128AOptionFloat64MenuItem", implementation::GlobalOptionItems::M128AViewDisplayFormatFloat64String, groupName));

    return menu;
}

Controls::MenuFlyoutSubItem UIHelper::CreateFloatingPointOption()
{
    auto const& rm = Microsoft::Windows::ApplicationModel::Resources::ResourceManager{};
    auto const menu = Controls::MenuFlyoutSubItem{};

    menu.Text(rm.MainResourceMap().GetValue(L"Resources/FloatingPointMenu").ValueAsString());

    GlobalOptionItems optionItems{};

    menu.Items().Append(CreateGlobalOptionsCheckedBoundPropertyItem(optionItems, L"Resources/FloatingPointOptionScientific", implementation::GlobalOptionItems::FloatingPointOptionScientificString));

    return menu;
}

void UIHelper::CreateSingleControlMenu(std::vector<Controls::MenuFlyoutItemBase> const& menus, Controls::TextBlock const& control)
{
    auto const menu = Controls::MenuFlyout{};

    menu.Items().Append(CreateCopyValue(control));
    menu.Items().Append(Controls::MenuFlyoutSeparator{});
    for(auto const& item : menus)
    {
        menu.Items().Append(item);
    }

    control.ContextFlyout(menu);
}
