#pragma once

namespace winrt::MiniDumpExplorer::implementation
{
    template<typename T>
    struct NotifyPropertyChangedBase  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    protected:
        NotifyPropertyChangedBase() = default;

    public:
        virtual ~NotifyPropertyChangedBase() = default;

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return propertyChanged_.add(value);
        }

        void PropertyChanged(event_token const& token)
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
            propertyChanged_(*Self(), Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
        }

    private:
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}