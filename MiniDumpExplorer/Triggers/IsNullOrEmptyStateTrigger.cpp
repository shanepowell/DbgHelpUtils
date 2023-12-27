#include "pch.h"
#include "IsNullOrEmptyStateTrigger.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#if __has_include("IsNullOrEmptyStateTrigger.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "IsNullOrEmptyStateTrigger.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    IsNullOrEmptyStateTrigger::IsNullOrEmptyStateTrigger() = default;

    Windows::Foundation::IInspectable IsNullOrEmptyStateTrigger::Value() const
    {
        return GetValue(ValueProperty());
    }

    void IsNullOrEmptyStateTrigger::Value(Windows::Foundation::IInspectable const& value) const
    {
        SetValue(ValueProperty(), value);
    }

    DependencyProperty IsNullOrEmptyStateTrigger::ValueProperty()
    {
        static DependencyProperty valueProperty =
            DependencyProperty::Register(
                L"Value",
                xaml_typename<Windows::Foundation::IInspectable>(),
                xaml_typename<MiniDumpExplorer::IsNullOrEmptyStateTrigger>(),
                PropertyMetadata{ nullptr, [](auto const& d, auto const& e) { d.template as<IsNullOrEmptyStateTrigger>()->OnValuePropertyChanged(e); } });

        return valueProperty;
    }

    void IsNullOrEmptyStateTrigger::OnValuePropertyChanged(DependencyPropertyChangedEventArgs const& args)
    {
        auto const value = args.NewValue();

        SetActive(IsNullOrEmpty(value));

        if(value == nullptr)
        {
            return;
        }

        // Try to listen for various notification events
        // Starting with INotifyCollectionChanged
        if (auto const valNotifyCollection = value.try_as<Interop::INotifyCollectionChanged>(); valNotifyCollection != nullptr)
        {
            valNotifyCollection.CollectionChanged([weak_ptr = get_weak()](auto const&, auto const&)
                {
                    if (auto const instance = weak_ptr.get())
                    {
                        instance->SetActive(IsNullOrEmpty(instance->Value()));
                    }
                });
            return;
        }

        // Not INotifyCollectionChanged, try IBindableObservableVector
        if (auto const valObservableVector = value.try_as<Interop::IBindableObservableVector>(); valObservableVector != nullptr)
        {
            valObservableVector.VectorChanged([weak_ptr = get_weak()](auto const&, auto const&)
                {
                    if (auto const instance = weak_ptr.get())
                    {
                        instance->SetActive(IsNullOrEmpty(instance->Value()));
                    }
                });
            return;
        }

        // Not INotifyCollectionChanged, try IObservableMap
        if (auto const valObservableVector = value.try_as<Windows::Foundation::Collections::IObservableMap<Windows::Foundation::IInspectable, Windows::Foundation::IInspectable>>(); valObservableVector != nullptr)
        {
            valObservableVector.MapChanged([weak_ptr = get_weak()](auto const&, auto const&)
                {
                    if (auto const instance = weak_ptr.get())
                    {
                        instance->SetActive(IsNullOrEmpty(instance->Value()));
                    }
                });
            return;
        }
    }

    bool IsNullOrEmptyStateTrigger::IsNullOrEmpty(Windows::Foundation::IInspectable const& value)
    {
        if (value == nullptr)
        {
            return true;
        }

        // Object is not null, check for an empty string
        if (auto const valString = value.try_as<hstring>(); valString.has_value())
        {
            return valString->empty();
        }

        // Object is not an IIterable, check for an empty IIterable for specific types
        if (auto const valEnumerable = value.try_as<Windows::Foundation::Collections::IIterable<Windows::Foundation::IInspectable>>(); valEnumerable != nullptr)
        {
            return std::begin(valEnumerable) == std::end(valEnumerable);
        }

        if (auto const valEnumerable = value.try_as<Windows::Foundation::Collections::IMap<Windows::Foundation::IInspectable, Windows::Foundation::IInspectable>>(); valEnumerable != nullptr)
        {
            return std::begin(valEnumerable) == std::end(valEnumerable);
        }

        // Not null and not a known type to test for emptiness
        return false;
    }
}
