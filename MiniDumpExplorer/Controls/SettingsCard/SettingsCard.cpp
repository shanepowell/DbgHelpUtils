#include "pch.h"
#include "SettingsCard.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "SettingsCardAutomationPeer.h"
#include "Helpers/ControlHelpers.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>

#if __has_include("SettingsCard.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsCard.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        const std::wstring NormalState = L"Normal"s;
        const std::wstring PointerOverState = L"PointerOver"s;
        const std::wstring PressedState = L"Pressed"s;
        const std::wstring DisabledState = L"Disabled"s;

        const std::wstring ActionIconPresenterHolder = L"PART_ActionIconPresenterHolder"s;
        const std::wstring HeaderPresenter = L"PART_HeaderPresenter"s;
        const std::wstring DescriptionPresenter = L"PART_DescriptionPresenter"s;
        const std::wstring HeaderIconPresenterHolder = L"PART_HeaderIconPresenterHolder"s;
    }

    SettingsCard::SettingsCard()
    {
        DefaultStyleKey(box_value(L"MiniDumpExplorer.SettingsCard"));
    }

    DependencyProperty SettingsCard::HeaderProperty()
    {
        static DependencyProperty headerProperty =
            DependencyProperty::Register(
                L"Header",
                xaml_typename<Windows::Foundation::IInspectable>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ nullptr, [](auto const& d, auto const& e) { d.template as<SettingsCard>()->OnHeaderPropertyChanged(e.OldValue(), e.NewValue()); } });

        return headerProperty;
    }

    DependencyProperty SettingsCard::DescriptionProperty()
    {
        static DependencyProperty descriptionProperty =
            DependencyProperty::Register(
                L"Description",
                xaml_typename<Windows::Foundation::IInspectable>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ nullptr, [](auto const& d, auto const& e) { d.template as<SettingsCard>()->OnDescriptionPropertyChanged(e.OldValue(), e.NewValue()); } });

        return descriptionProperty;
    }

    DependencyProperty SettingsCard::HeaderIconProperty()
    {
        static DependencyProperty headerIconProperty =
            DependencyProperty::Register(
                L"HeaderIcon",
                xaml_typename<Controls::IconElement>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ nullptr, [](auto const& d, auto const& e) { d.template as<SettingsCard>()->OnHeaderIconPropertyChanged(e.OldValue().template as<Controls::IconElement>(), e.NewValue().template as<Controls::IconElement>()); } });

        return headerIconProperty;
    }

    DependencyProperty SettingsCard::ActionIconProperty()
    {
        static DependencyProperty actionIconProperty =
            DependencyProperty::Register(
                L"ActionIcon",
                xaml_typename<Controls::IconElement>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ MakeDefaultActionIcon() });

        return actionIconProperty;
    }

    DependencyProperty SettingsCard::ActionIconToolTipProperty()
    {
        static DependencyProperty actionIconToolTipProperty =
            DependencyProperty::Register(
                L"ActionIconToolTip",
                xaml_typename<hstring>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ box_value(L"More") });

        return actionIconToolTipProperty;
    }

    DependencyProperty SettingsCard::IsClickEnabledProperty()
    {
        static DependencyProperty isClickEnabledProperty =
            DependencyProperty::Register(
                L"IsClickEnabled",
                xaml_typename<bool>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ box_value(false), [](auto const& d, auto const& e) { d.template as<SettingsCard>()->OnIsClickEnabledPropertyChanged(unbox_value<bool>(e.OldValue()), unbox_value<bool>(e.NewValue())); } });

        return isClickEnabledProperty;
    }

    DependencyProperty SettingsCard::ContentAlignmentProperty()
    {
        static DependencyProperty contentAlignmentProperty =
            DependencyProperty::Register(
                L"ContentAlignment",
                xaml_typename<MiniDumpExplorer::ContentAlignment>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ box_value(ContentAlignment::Right) });

        return contentAlignmentProperty;
    }

    DependencyProperty SettingsCard::IsActionIconVisibleProperty()
    {
        static DependencyProperty isActionIconVisibleProperty =
            DependencyProperty::Register(
                L"IsActionIconVisible",
                xaml_typename<bool>(),
                xaml_typename<MiniDumpExplorer::SettingsCard>(),
                PropertyMetadata{ box_value(true), [](auto const& d, auto const& e) { d.template as<SettingsCard>()->OnIsActionIconVisiblePropertyChanged(unbox_value<bool>(e.OldValue()), unbox_value<bool>(e.NewValue())); } });

        return isActionIconVisibleProperty;
    }

    Windows::Foundation::IInspectable SettingsCard::Header() const
    {
        return GetValue(HeaderProperty());
    }

    void SettingsCard::Header(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(HeaderProperty(), value);
    }

    Windows::Foundation::IInspectable SettingsCard::Description() const
    {
        return GetValue(DescriptionProperty());
    }

    void SettingsCard::Description(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(DescriptionProperty(), value);
    }

    Controls::IconElement SettingsCard::HeaderIcon() const
    {
        return GetValue(HeaderIconProperty()).as<Controls::IconElement>();
    }

    void SettingsCard::HeaderIcon(Controls::IconElement const& value) const
    {
        SetValue(HeaderIconProperty(), value);
    }

    Controls::IconElement SettingsCard::ActionIcon() const
    {
        return GetValue(ActionIconProperty()).as<Controls::IconElement>();
    }

    void SettingsCard::ActionIcon(Controls::IconElement const& value) const
    {
        SetValue(ActionIconProperty(), value);
    }

    hstring SettingsCard::ActionIconToolTip() const
    {
        return unbox_value<hstring>(GetValue(ActionIconToolTipProperty()));
    }

    void SettingsCard::ActionIconToolTip(hstring const& value) const
    {
        SetValue(ActionIconToolTipProperty(), box_value(value));
    }

    bool SettingsCard::IsClickEnabled() const
    {
        return unbox_value<bool>(GetValue(IsClickEnabledProperty()));
    }

    void SettingsCard::IsClickEnabled(bool const value) const
    {
        SetValue(IsClickEnabledProperty(), box_value(value));
    }

    ContentAlignment SettingsCard::ContentAlignment() const
    {
        return unbox_value<MiniDumpExplorer::ContentAlignment>(GetValue(ContentAlignmentProperty()));
    }

    void SettingsCard::ContentAlignment(MiniDumpExplorer::ContentAlignment const value) const
    {
        SetValue(ContentAlignmentProperty(), box_value(value));
    }

    bool SettingsCard::IsActionIconVisible() const
    {
        return unbox_value<bool>(GetValue(IsActionIconVisibleProperty()));
    }

    void SettingsCard::IsActionIconVisible(bool const value) const
    {
        SetValue(IsActionIconVisibleProperty(), box_value(value));
    }

    void SettingsCard::OnIsClickEnabledPropertyChanged([[maybe_unused]] bool oldValue, [[maybe_unused]] bool newValue)
    {
        OnIsClickEnabledChanged();
    }

    void SettingsCard::OnHeaderIconPropertyChanged([[maybe_unused]] Controls::IconElement const& oldValue, [[maybe_unused]] Controls::IconElement const& newValue)
    {
        OnHeaderIconChanged();
    }

    void SettingsCard::OnHeaderPropertyChanged([[maybe_unused]] Windows::Foundation::IInspectable const& oldValue, [[maybe_unused]] Windows::Foundation::IInspectable const& newValue)
    {
        OnHeaderChanged();
    }

    void SettingsCard::OnDescriptionPropertyChanged([[maybe_unused]] Windows::Foundation::IInspectable const& oldValue, [[maybe_unused]] Windows::Foundation::IInspectable const& newValue)
    {
        OnDescriptionChanged();
    }

    void SettingsCard::OnIsActionIconVisiblePropertyChanged([[maybe_unused]] bool oldValue, [[maybe_unused]] bool newValue)
    {
        OnActionIconChanged();
    }

    void SettingsCard::OnApplyTemplate()
    {
        if(isEnabledChangedToken_)
        {
            IsEnabledChanged(isEnabledChangedToken_);
            isEnabledChangedToken_ = event_token{};
        }
        OnActionIconChanged();
        OnHeaderChanged();
        OnHeaderIconChanged();
        OnDescriptionChanged();
        OnIsClickEnabledChanged();
        VisualStateManager::GoToState(*this, IsEnabled() ? NormalState : DisabledState, true);
        RegisterAutomation();
        isEnabledChangedToken_ = IsEnabledChanged(DependencyPropertyChangedEventHandler{this, &SettingsCard::OnIsEnabledChanged});
    }

    void SettingsCard::RegisterAutomation() const
    {
        if (auto const header = Header().try_as<hstring>(); header.has_value() && !header.value().empty())
        {
            Automation::AutomationProperties::SetName(*this, header.value());
            // We don't want to override an AutomationProperties.Name that is manually set, or if the Content base type is of type ButtonBase (the ButtonBase.Content will be used then)
            if (auto const element = Content().try_as<UIElement>(); element 
                && Automation::AutomationProperties::GetName(element).empty() 
                && element.try_as<Controls::Primitives::ButtonBase>() == nullptr
                && element.try_as<Controls::TextBlock>() == nullptr)
            {
                Automation::AutomationProperties::SetName(element, header.value());
            }
        }
    }

    void SettingsCard::EnableButtonInteraction()
    {
        DisableButtonInteraction();

        IsTabStop(true);
        pointerEnteredToken_ = PointerEntered(Input::PointerEventHandler{this, &SettingsCard::Control_PointerEntered});
        pointerExitedToken_ = PointerExited(Input::PointerEventHandler{this, &SettingsCard::Control_PointerExited});
        pointerCaptureLostToken_ = PointerCaptureLost(Input::PointerEventHandler{this, &SettingsCard::Control_PointerCaptureLost});
        pointerCanceledToken_ = PointerCanceled(Input::PointerEventHandler{this, &SettingsCard::Control_PointerCanceled});
        previewKeyDownToken_ = PreviewKeyDown(Input::KeyEventHandler{this, &SettingsCard::Control_PreviewKeyDown});
        previewKeyUpToken_ = PreviewKeyUp(Input::KeyEventHandler{this, &SettingsCard::Control_PreviewKeyUp});
    }

    void SettingsCard::DisableButtonInteraction()
    {
        IsTabStop(false);
        if(pointerEnteredToken_)
        {
            PointerEntered(pointerEnteredToken_);
            pointerEnteredToken_ = event_token{};
        }
        if(pointerExitedToken_)
        {
            PointerExited(pointerExitedToken_);
            pointerExitedToken_ = event_token{};
        }
        if(pointerCaptureLostToken_)
        {
            PointerCaptureLost(pointerCaptureLostToken_);
            pointerCaptureLostToken_ = event_token{};
        }
        if(pointerCanceledToken_)
        {
            PointerCanceled(pointerCanceledToken_);
            pointerCanceledToken_ = event_token{};
        }
        if(previewKeyDownToken_)
        {
            PreviewKeyDown(previewKeyDownToken_);
            previewKeyDownToken_ = event_token{};
        }
        if(previewKeyUpToken_)
        {
            PreviewKeyUp(previewKeyUpToken_);
            previewKeyUpToken_ = event_token{};
        }
    }

    void SettingsCard::Control_PreviewKeyUp([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e) const
    {
        switch(e.Key())
        {
            case Windows::System::VirtualKey::Enter:
            case Windows::System::VirtualKey::Space:
            case Windows::System::VirtualKey::GamepadA:
            VisualStateManager::GoToState(*this, NormalState, true);
            break;

        default:
            break;
        }
    }

    void SettingsCard::Control_PreviewKeyDown([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::KeyRoutedEventArgs const& e) const
    {
        switch(e.Key())
        {
            case Windows::System::VirtualKey::Enter:
            case Windows::System::VirtualKey::Space:
            case Windows::System::VirtualKey::GamepadA:
            // Check if the active focus is on the card itself - only then we show the pressed state.
            if (GetFocusedElement().try_as<SettingsCard>())
            {
                VisualStateManager::GoToState(*this, PressedState, true);
            }
            break;

        default:
            break;
        }
    }

    void SettingsCard::Control_PointerEntered([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) const
    {
        OnPointerEntered(e);
        VisualStateManager::GoToState(*this, PointerOverState, true);
    }

    void SettingsCard::Control_PointerExited([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) const
    {
        OnPointerExited(e);
        VisualStateManager::GoToState(*this, NormalState, true);
    }
    
    void SettingsCard::Control_PointerCaptureLost([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) const
    {
        OnPointerCaptureLost(e);
        VisualStateManager::GoToState(*this, NormalState, true);
    }

    void SettingsCard::Control_PointerCanceled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) const
    {
        OnPointerCanceled(e);
        VisualStateManager::GoToState(*this, NormalState, true);
    }

    void SettingsCard::OnPointerPressed(Input::PointerRoutedEventArgs const& e) const
    {
        if (IsClickEnabled())
        {
            base_type::OnPointerPressed(e);
            VisualStateManager::GoToState(*this, PressedState, true);
        }
    }

    void SettingsCard::OnPointerReleased(Input::PointerRoutedEventArgs const& e) const
    {
        if (IsClickEnabled())
        {
            base_type::OnPointerReleased(e);
            VisualStateManager::GoToState(*this, NormalState, true);
        }
    }

    void SettingsCard::OnIsClickEnabledChanged()
    {
        OnActionIconChanged();
        if (IsClickEnabled())
        {
            EnableButtonInteraction();
        }
        else
        {
            DisableButtonInteraction();
        }
    }

    void SettingsCard::OnIsEnabledChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        VisualStateManager::GoToState(*this, IsEnabled() ? NormalState : DisabledState, true);
    }

    void SettingsCard::OnActionIconChanged() const
    {
        if (auto const actionIconPresenter = GetTemplateChild(ActionIconPresenterHolder).try_as<FrameworkElement>(); actionIconPresenter)
        {
            if (IsClickEnabled() && IsActionIconVisible())
            {
                actionIconPresenter.Visibility(Visibility::Visible);
            }
            else
            {
                actionIconPresenter.Visibility(Visibility::Collapsed);
            }
        }
    }

    void SettingsCard::OnHeaderIconChanged() const
    {
        if (auto const headerIconPresenter = GetTemplateChild(HeaderIconPresenterHolder).try_as<FrameworkElement>(); headerIconPresenter)
        {
            headerIconPresenter.Visibility(HeaderIcon() != nullptr ? Visibility::Visible : Visibility::Collapsed);
        }
    }

    void SettingsCard::OnDescriptionChanged() const
    {
        if (auto const descriptionPresenter = GetTemplateChild(DescriptionPresenter).try_as<FrameworkElement>(); descriptionPresenter)
        {
            descriptionPresenter.Visibility(Description() != nullptr ? Visibility::Visible : Visibility::Collapsed);
        }
    }

    void SettingsCard::OnHeaderChanged() const
    {
        if (auto const headerPresenter = GetTemplateChild(HeaderPresenter).try_as<FrameworkElement>(); headerPresenter)
        {
            headerPresenter.Visibility(Header() != nullptr ? Visibility::Visible : Visibility::Collapsed);
        }
    }

    Automation::Peers::AutomationPeer SettingsCard::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::SettingsCardAutomationPeer{*this};
    }

    FrameworkElement SettingsCard::GetFocusedElement() const
    {
        if (ControlHelpers::IsXamlRootAvailable() && XamlRoot() != nullptr)
        {
            return Input::FocusManager::GetFocusedElement(XamlRoot()).as<FrameworkElement>();
        }

        return Input::FocusManager::GetFocusedElement().as<FrameworkElement>();
    }

    Controls::IconElement SettingsCard::MakeDefaultActionIcon()
    {
        return Controls::SymbolIcon{ static_cast<Controls::Symbol>(0xe974) };
    }
}
