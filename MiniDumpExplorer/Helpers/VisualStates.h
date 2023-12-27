#pragma once

class VisualStates
{
public:
    static const winrt::hstring StateNormal;
    static const winrt::hstring StatePointerOver;
    static const winrt::hstring StatePressed;
    static const winrt::hstring StateDisabled;
    static const winrt::hstring GroupCommon;
    static const winrt::hstring StateExpanded;
    static const winrt::hstring StateCollapsed;
    static const winrt::hstring StateEmpty;
    static const winrt::hstring StateUnfocused;
    static const winrt::hstring StateFocused;
    static const winrt::hstring GroupFocus;
    static const winrt::hstring StateSelected;
    static const winrt::hstring StateUnselected;
    static const winrt::hstring GroupSelection;
    static const winrt::hstring StateActive;
    static const winrt::hstring StateInactive;
    static const winrt::hstring GroupActive;
    static const winrt::hstring StateRegular;
    static const winrt::hstring StateCurrent;
    static const winrt::hstring StateCurrentWithFocus;
    static const winrt::hstring GroupCurrent;
    static const winrt::hstring StateDisplay;
    static const winrt::hstring StateEditing;
    static const winrt::hstring GroupInteraction;
    static const winrt::hstring StateUnsorted;
    static const winrt::hstring StateSortAscending;
    static const winrt::hstring StateSortDescending;
    static const winrt::hstring GroupSort;
    static const winrt::hstring StateInvalid;
    static const winrt::hstring StateRowInvalid;
    static const winrt::hstring StateRowValid;
    static const winrt::hstring StateValid;
    static const winrt::hstring GroupValidation;
    static const winrt::hstring StateSeparatorExpanded;
    static const winrt::hstring StateSeparatorCollapsed;
    static const winrt::hstring StateSeparatorExpandedWithoutAnimation;
    static const winrt::hstring StateSeparatorCollapsedWithoutAnimation;
    static const winrt::hstring GroupScrollBarsSeparator;
    static const winrt::hstring StateTouchIndicator;
    static const winrt::hstring StateMouseIndicator;
    static const winrt::hstring StateMouseIndicatorFull;
    static const winrt::hstring StateNoIndicator;
    static const winrt::hstring GroupScrollBars;

    template<typename ...T>
    static void GoToState(winrt::Microsoft::UI::Xaml::Controls::Control const& control, bool useTransitions, T const&... stateNames)
    {
        assert(control);

        for (auto const& stateName : { stateNames... })
        {
            if (winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(control, stateName, useTransitions))
            {
                break;
            }
        }
    }
};
