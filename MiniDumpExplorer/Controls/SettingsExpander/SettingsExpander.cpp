#include "pch.h"

#include "App.xaml.h"

#include "SettingsExpander.h"
#if __has_include("SettingsExpander.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsExpander.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        const std::wstring ItemsRepeater = L"PART_ItemsRepeater"s;
    }

    SettingsExpander::SettingsExpander()
    {
        DefaultStyleKey(box_value(L"MiniDumpExplorer.SettingsExpander"));
        HeaderProperty();
        DescriptionProperty();
        HeaderIconProperty();
        ItemsHeaderProperty();
        ItemsFooterProperty();
        IsExpandedProperty();
        ItemsProperty();
        ItemsSourceProperty();
        ItemTemplateProperty();
        ItemContainerStyleSelectorProperty();
        ContentProperty();
        Items(single_threaded_vector<Windows::Foundation::IInspectable>());
    }

    event_token SettingsExpander::Collapsed(Windows::Foundation::EventHandler<RoutedEventArgs> const& value)
    {
        return collapsedHandler_.add(value);
    }

    void SettingsExpander::Collapsed(event_token const& value)
    {
        collapsedHandler_.remove(value);
    }

    event_token SettingsExpander::Expanded(Windows::Foundation::EventHandler<RoutedEventArgs> const& value)
    {
        return expandedHandler_.add(value);
    }

    void SettingsExpander::Expanded(event_token const& value)
    {
        expandedHandler_.remove(value);
    }

    DependencyProperty SettingsExpander::HeaderProperty()
    {
        static DependencyProperty s_headerProperty =
            DependencyProperty::Register(
                L"Header",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_headerProperty;
    }

    DependencyProperty SettingsExpander::DescriptionProperty()
    {
        static DependencyProperty s_descriptionProperty =
            DependencyProperty::Register(
                L"Description",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_descriptionProperty;
    }

    DependencyProperty SettingsExpander::HeaderIconProperty()
    {
        static DependencyProperty s_headerIconProperty =
            DependencyProperty::Register(
                L"HeaderIcon",
                winrt::xaml_typename<Controls::IconElement>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_headerIconProperty;
    }

    DependencyProperty SettingsExpander::ItemsHeaderProperty()
    {
        static DependencyProperty s_itemsHeaderProperty =
            DependencyProperty::Register(
                L"ItemsHeader",
                winrt::xaml_typename<UIElement>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_itemsHeaderProperty;
    }

    DependencyProperty SettingsExpander::ItemsFooterProperty()
    {
        static DependencyProperty s_itemsFooterProperty =
            DependencyProperty::Register(
                L"ItemsFooter",
                winrt::xaml_typename<UIElement>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_itemsFooterProperty;
    }

    DependencyProperty SettingsExpander::IsExpandedProperty()
    {
        static DependencyProperty s_isExpandedProperty =
            DependencyProperty::Register(
                L"IsExpanded",
                winrt::xaml_typename<bool>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ box_value(false), PropertyChangedCallback{ [](auto const& d, auto const& e) { d.template as<SettingsExpander>()->OnIsExpandedChanged(unbox_value<bool>(e.OldValue()), unbox_value<bool>(e.NewValue())); } } });

        return s_isExpandedProperty;
    }

    DependencyProperty SettingsExpander::ItemsProperty()
    {
        static DependencyProperty s_itemsProperty =
            DependencyProperty::Register(
                L"Items",
                winrt::xaml_typename<Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable>>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr, PropertyChangedCallback{ [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<SettingsExpander>()->OnItemsConnectedPropertyChanged(); } } });

        return s_itemsProperty;
    }

    DependencyProperty SettingsExpander::ItemsSourceProperty()
    {
        static DependencyProperty s_itemsSourceProperty =
            DependencyProperty::Register(
                L"ItemsSource",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr, PropertyChangedCallback{ [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<SettingsExpander>()->OnItemsConnectedPropertyChanged(); } } });

        return s_itemsSourceProperty;
    }

    DependencyProperty SettingsExpander::ItemTemplateProperty()
    {
        static DependencyProperty s_itemTemplateProperty =
            DependencyProperty::Register(
                L"ItemTemplate",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr, PropertyChangedCallback{ [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<SettingsExpander>()->OnItemsConnectedPropertyChanged(); } } });

        return s_itemTemplateProperty;
    }

    DependencyProperty SettingsExpander::ItemContainerStyleSelectorProperty()
    {
        static DependencyProperty s_itemContainerStyleSelectorProperty =
            DependencyProperty::Register(
                L"ItemContainerStyleSelector",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_itemContainerStyleSelectorProperty;
    }

    DependencyProperty SettingsExpander::ContentProperty()
    {
        static DependencyProperty s_contentProperty =
            DependencyProperty::Register(
                L"Content",
                winrt::xaml_typename<Windows::Foundation::IInspectable>(),
                winrt::xaml_typename<MiniDumpExplorer::SettingsExpander>(),
                PropertyMetadata{ nullptr });

        return s_contentProperty;
    }

    Windows::Foundation::IInspectable SettingsExpander::Header() const
    {
        return GetValue(HeaderProperty());
    }

    void SettingsExpander::Header(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(HeaderProperty(), value);
    }

    Windows::Foundation::IInspectable SettingsExpander::Description() const
    {
        return GetValue(DescriptionProperty());
    }

    void SettingsExpander::Description(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(DescriptionProperty(), value);
    }

    Controls::IconElement SettingsExpander::HeaderIcon() const
    {
        return GetValue(HeaderIconProperty()).as<Controls::IconElement>();
    }

    void SettingsExpander::HeaderIcon(Controls::IconElement const& value) const
    {
        SetValue(HeaderIconProperty(), value);
    }

    UIElement SettingsExpander::ItemsHeader() const
    {
        return GetValue(ItemsHeaderProperty()).as<UIElement>();
    }

    void SettingsExpander::ItemsHeader(UIElement const& value) const
    {
        SetValue(ItemsHeaderProperty(), value);
    }

    UIElement SettingsExpander::ItemsFooter() const
    {
        return GetValue(ItemsFooterProperty()).as<UIElement>();
    }

    void SettingsExpander::ItemsFooter(UIElement const& value) const
    {
        SetValue(ItemsFooterProperty(), value);
    }

    bool SettingsExpander::IsExpanded() const
    {
        return unbox_value<bool>(GetValue(IsExpandedProperty()));
    }

    void SettingsExpander::IsExpanded(bool const value) const
    {
        SetValue(IsExpandedProperty(), box_value(value));
    }

    Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> SettingsExpander::Items() const
    {
        return GetValue(ItemsProperty()).as<Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable>>();
    }

    void SettingsExpander::Items(Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> const& value) const
    {
        SetValue(ItemsProperty(), value);
    }

    Windows::Foundation::IInspectable SettingsExpander::ItemsSource() const
    {
        return GetValue(ItemsSourceProperty());
    }

    void SettingsExpander::ItemsSource(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ItemsSourceProperty(), value);
    }

    Windows::Foundation::IInspectable SettingsExpander::ItemTemplate() const
    {
        return GetValue(ItemTemplateProperty());
    }

    void SettingsExpander::ItemTemplate(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ItemTemplateProperty(), value);
    }

    Controls::StyleSelector SettingsExpander::ItemContainerStyleSelector() const
    {
        return GetValue(ItemContainerStyleSelectorProperty()).as<Controls::StyleSelector>();
    }

    void SettingsExpander::ItemContainerStyleSelector(Controls::StyleSelector const& value) const
    {
        SetValue(ItemContainerStyleSelectorProperty(), value);
    }

    Windows::Foundation::IInspectable SettingsExpander::Content() const
    {
        return GetValue(ContentProperty());
    }

    void SettingsExpander::Content(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ContentProperty(), value);
    }

    void SettingsExpander::OnApplyTemplate()
    {
        RegisterAutomation();

        if (itemsRepeater_ && itemsRepeaterElementPreparedToken_)
        {
            itemsRepeater_.ElementPrepared(itemsRepeaterElementPreparedToken_);
        }

        itemsRepeater_ = GetTemplateChild(ItemsRepeater).as<Controls::ItemsRepeater>();

        if (itemsRepeater_)
        {
            itemsRepeaterElementPreparedToken_ = itemsRepeater_.ElementPrepared({this, &SettingsExpander::ItemsRepeater_ElementPrepared});

            // Update it's source based on our current items properties.
            OnItemsConnectedPropertyChanged();
        }
    }

    Automation::Peers::AutomationPeer SettingsExpander::OnCreateAutomationPeer() const
    {
        return make<SettingsExpanderAutomationPeer>(*this);
    }

    void SettingsExpander::OnIsExpandedPropertyChanged(bool const oldValue, bool const newValue)
    {
        OnIsExpandedChanged(oldValue, newValue);

        if (newValue)  // NOLINT(bugprone-branch-clone)
        {
            expandedHandler_(*this, RoutedEventArgs{});
        }
        else
        {
            collapsedHandler_(*this, RoutedEventArgs{});
        }
    }

    void SettingsExpander::RegisterAutomation() const
    {
        if (auto const header = Header().try_as<hstring>(); header.has_value() && !header.value().empty())
        {
            if (Automation::AutomationProperties::GetName(*this).empty())
            {
                Automation::AutomationProperties::SetName(*this, header.value());
            }
        }
    }

    void SettingsExpander::OnIsExpandedChanged([[maybe_unused]] bool const oldValue, bool const newValue) const
    {
        if(auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<SettingsExpanderAutomationPeer>(); peer)
        {
            peer->RaiseExpandedChangedEvent(newValue);
        }
    }

    void SettingsExpander::OnItemsConnectedPropertyChanged() const
    {
        if (itemsRepeater_ != nullptr)
        {
            auto dataSource = ItemsSource();
            if (!dataSource)
            {
                dataSource = Items();
            }

            itemsRepeater_.ItemsSource(dataSource);
        }
    }

    void SettingsExpander::ItemsRepeater_ElementPrepared([[maybe_unused]]Controls::ItemsRepeater const& sender, Controls::ItemsRepeaterElementPreparedEventArgs const& args) const
    {
        
        if (auto const selector = ItemContainerStyleSelector(); selector)
        {
            
            if(auto const element = args.Element().try_as<FrameworkElement>();
                element && element.ReadLocalValue(FrameworkElement::StyleProperty()) == DependencyProperty::UnsetValue())
            {
                element.Style(selector.SelectStyle(nullptr, element));
            }
        }
    }
}
