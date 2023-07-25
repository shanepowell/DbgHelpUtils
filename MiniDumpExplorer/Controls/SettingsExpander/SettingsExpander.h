// ReSharper disable CppHidingFunction
#pragma once

#include "SettingsExpander.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsExpander : SettingsExpanderT<SettingsExpander>
    {
        SettingsExpander();

        event_token Collapsed(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs> const& value);
        void Collapsed(event_token const& value);
        event_token Expanded(Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs> const& value);
        void Expanded(event_token const& value);
        static Microsoft::UI::Xaml::DependencyProperty HeaderProperty();
        static Microsoft::UI::Xaml::DependencyProperty DescriptionProperty();
        static Microsoft::UI::Xaml::DependencyProperty HeaderIconProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemsHeaderProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemsFooterProperty();
        static Microsoft::UI::Xaml::DependencyProperty IsExpandedProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemsProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemsSourceProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemTemplateProperty();
        static Microsoft::UI::Xaml::DependencyProperty ItemContainerStyleSelectorProperty();
        static Microsoft::UI::Xaml::DependencyProperty ContentProperty();
        Windows::Foundation::IInspectable Header() const;
        void Header(Windows::Foundation::IInspectable const& value) const;
        Windows::Foundation::IInspectable Description() const;
        void Description(Windows::Foundation::IInspectable const& value) const;
        Microsoft::UI::Xaml::Controls::IconElement HeaderIcon() const;
        void HeaderIcon(Microsoft::UI::Xaml::Controls::IconElement const& value) const;
        Microsoft::UI::Xaml::UIElement ItemsHeader() const;
        void ItemsHeader(Microsoft::UI::Xaml::UIElement const& value) const;
        Microsoft::UI::Xaml::UIElement ItemsFooter() const;
        void ItemsFooter(Microsoft::UI::Xaml::UIElement const& value) const;
        bool IsExpanded() const;
        void IsExpanded(bool value) const;
        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Items() const;
        void Items(Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> const& value) const;
        Windows::Foundation::IInspectable ItemsSource() const;
        void ItemsSource(Windows::Foundation::IInspectable const& value) const;
        Windows::Foundation::IInspectable ItemTemplate() const;
        void ItemTemplate(Windows::Foundation::IInspectable const& value) const;
        Microsoft::UI::Xaml::Controls::StyleSelector ItemContainerStyleSelector() const;
        void ItemContainerStyleSelector(Microsoft::UI::Xaml::Controls::StyleSelector const& value) const;
        Windows::Foundation::IInspectable Content() const;
        void Content(Windows::Foundation::IInspectable const& value) const;

        void OnApplyTemplate();
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const;

    protected:
        virtual void OnIsExpandedPropertyChanged(bool oldValue, bool newValue);

    private:
        void RegisterAutomation() const;
        void OnIsExpandedChanged(bool oldValue, bool newValue) const;
        void OnItemsConnectedPropertyChanged() const;
        void ItemsRepeater_ElementPrepared(Microsoft::UI::Xaml::Controls::ItemsRepeater const& sender, Microsoft::UI::Xaml::Controls::ItemsRepeaterElementPreparedEventArgs const& args) const;

    private:
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs>> collapsedHandler_;
        event<Windows::Foundation::EventHandler<Microsoft::UI::Xaml::RoutedEventArgs>> expandedHandler_;
        Microsoft::UI::Xaml::Controls::ItemsRepeater itemsRepeater_{ nullptr };
        event_token itemsRepeaterElementPreparedToken_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsExpander : SettingsExpanderT<SettingsExpander, implementation::SettingsExpander>
    {
    };
}
