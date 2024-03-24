#pragma once

template<typename T>
struct NotifyPropertyChangedBase
{
protected:
    NotifyPropertyChangedBase() = default;

public:
    winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
    {
        return propertyChanged_.add(value);
    }

    void PropertyChanged(winrt::event_token const& token)
    {
        propertyChanged_.remove(token);
    }

protected:
    T* Self() noexcept
    {
        return static_cast<T*>(this);
    }

    void RaisePropertyChanged(std::wstring_view const& propertyName)
    {
        propertyChanged_(*Self(), winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
    }

private:
    winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
};
