#pragma once

#include <winrt/Microsoft.UI.Xaml.Input.h>

namespace Utility
{
    template<typename TExecute, typename TCanExecute>
    struct RelayCommand : winrt::implements<RelayCommand<TExecute, TCanExecute>, winrt::Microsoft::UI::Xaml::Input::ICommand>
    {
        RelayCommand(TExecute execute, TCanExecute canExecute)
            : execute_(std::move(execute))
            , canExecute_(std::move(canExecute))
        {
        }

        bool CanExecute(winrt::Windows::Foundation::IInspectable const& parameter)
        {
            auto const canExecute = canExecute_(parameter);
            if(canExecute != currentCanExecute_)
            {
                currentCanExecute_ = canExecute;
                canExecuteChangedEvent_(*this, nullptr);
            }
            return canExecute;
        }

        winrt::fire_and_forget Execute(winrt::Windows::Foundation::IInspectable const& parameter)
        {
            co_await execute_(parameter);
        }

        using Delegate = winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>;

        winrt::event_token CanExecuteChanged(Delegate const& handler)
        {
            return canExecuteChangedEvent_.add(handler);
        }

        void CanExecuteChanged(winrt::event_token const& token)
        {
            canExecuteChangedEvent_.remove(token);
        }

    private:
        TExecute execute_;
        TCanExecute canExecute_;
        winrt::event<Delegate> canExecuteChangedEvent_;
        bool currentCanExecute_ = false;
    };

    template<typename TExecute>
    auto MakeRelayCommand(TExecute execute)
    {
        auto canExecute = [](auto const&) { return true; };
        return winrt::make<RelayCommand<TExecute, decltype(canExecute)>>(std::move(execute), std::move(canExecute));
    }

    template<typename TExecute, typename TCanExecute>
    auto MakeRelayCommand(TExecute execute, TCanExecute canExecute)
    {
        return winrt::make<RelayCommand<TExecute, TCanExecute>>(std::move(execute), std::move(canExecute));
    }
}