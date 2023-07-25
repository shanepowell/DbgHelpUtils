// ReSharper disable CppHidingFunction
#pragma once

#include "SettingsCard.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsCard : SettingsCardT<SettingsCard>
    {
        SettingsCard();

        static void Initialize();

        static Microsoft::UI::Xaml::DependencyProperty HeaderProperty();
        static Microsoft::UI::Xaml::DependencyProperty DescriptionProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeaderIconProperty();
        static Microsoft::UI::Xaml::DependencyProperty ActionIconProperty();
        static Microsoft::UI::Xaml::DependencyProperty ActionIconToolTipProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsClickEnabledProperty();
        static Microsoft::UI::Xaml::DependencyProperty ContentAlignmentProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsActionIconVisibleProperty();
        Windows::Foundation::IInspectable Header() const;
        void Header(Windows::Foundation::IInspectable const& value) const;
        Windows::Foundation::IInspectable Description() const;
        void Description(Windows::Foundation::IInspectable const& value) const;
        Microsoft::UI::Xaml::Controls::IconElement HeaderIcon() const;
        void HeaderIcon(Microsoft::UI::Xaml::Controls::IconElement const& value) const;
        Microsoft::UI::Xaml::Controls::IconElement ActionIcon() const;
        void ActionIcon(Microsoft::UI::Xaml::Controls::IconElement const& value) const;
        hstring ActionIconToolTip() const;
        void ActionIconToolTip(hstring const& value) const;
        bool IsClickEnabled() const;
        void IsClickEnabled(bool value) const;
        ContentAlignment ContentAlignment() const;
        void ContentAlignment(MiniDumpExplorer::ContentAlignment value) const;
        bool IsActionIconVisible() const;
        void IsActionIconVisible(bool value) const;

        void OnApplyTemplate();
        void OnPointerPressed(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void OnPointerReleased(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const;

    protected:
        virtual void OnIsClickEnabledPropertyChanged(bool oldValue, bool newValue);
        virtual void OnHeaderIconPropertyChanged(Microsoft::UI::Xaml::Controls::IconElement const& oldValue, Microsoft::UI::Xaml::Controls::IconElement const& newValue);
        virtual void OnHeaderPropertyChanged(Windows::Foundation::IInspectable const& oldValue, Windows::Foundation::IInspectable const& newValue);
        virtual void OnDescriptionPropertyChanged(Windows::Foundation::IInspectable const& oldValue, Windows::Foundation::IInspectable const& newValue);
        virtual void OnIsActionIconVisiblePropertyChanged(bool oldValue, bool newValue);

    private:
        void RegisterAutomation() const;
        void EnableButtonInteraction();
        void DisableButtonInteraction();
        void Control_PreviewKeyUp(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e) const;
        void Control_PreviewKeyDown(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e) const;
        void Control_PointerEntered(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void Control_PointerExited(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void Control_PointerCaptureLost(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void Control_PointerCanceled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void OnIsClickEnabledChanged();
        void OnIsEnabledChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnActionIconChanged() const;
        void OnHeaderIconChanged() const;
        void OnDescriptionChanged() const;
        void OnHeaderChanged() const;

        Microsoft::UI::Xaml::FrameworkElement GetFocusedElement() const;

        static Microsoft::UI::Xaml::Controls::IconElement MakeDefaultActionIcon();

    private:
        event_token isEnabledChangedToken_;
        event_token pointerEnteredToken_;
        event_token pointerExitedToken_;
        event_token pointerCaptureLostToken_;
        event_token pointerCanceledToken_;
        event_token previewKeyDownToken_;
        event_token previewKeyUpToken_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsCard : SettingsCardT<SettingsCard, implementation::SettingsCard>
    {
    };
}
