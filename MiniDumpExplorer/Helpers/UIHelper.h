#pragma once

class UIHelper
{
public:
    static void AnnounceActionForAccessibility(winrt::Microsoft::UI::Xaml::UIElement const& ue, winrt::hstring const& announcement, winrt::hstring const& activityID);
    static void CopyToClipboard(winrt::hstring const& text);
    static void EnsureMeasured(winrt::Microsoft::UI::Xaml::UIElement const& element);
    static void SetStyleWithType(winrt::Microsoft::UI::Xaml::FrameworkElement const& element, winrt::Microsoft::UI::Xaml::Style const& style);
    static winrt::Windows::Foundation::Point Translate(winrt::Microsoft::UI::Xaml::UIElement const& fromElement, winrt::Microsoft::UI::Xaml::UIElement const& toElement, winrt::Windows::Foundation::Point const& fromPoint);
    static bool ContainsChild(winrt::Microsoft::UI::Xaml::DependencyObject const& element, winrt::Microsoft::UI::Xaml::DependencyObject child);
    static bool ContainsFocusedElement(winrt::Microsoft::UI::Xaml::DependencyObject const& element, winrt::Microsoft::UI::Xaml::UIElement const& uiElement);
    static winrt::hstring ObjectToString(winrt::Windows::Foundation::IInspectable const& object);
    static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem CreateCopyValue(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& control);
    static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem CreateHexNumberDisplayFormatOption();
    static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem CreateSizeNumberDisplayUnitFormatOption();
    static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem CreateSizeNumberDisplayPrintFormatOption();
    static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem CreateSizeNumberDisplayBaseOption();
    static void CreateStandardHexNumberMenu(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& control);
    static void CreateStandardSizeNumberMenu(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& control);
};
