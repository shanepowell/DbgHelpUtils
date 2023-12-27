#pragma once
#include <functional>

namespace Utility
{
    template<typename TInstance, typename TSource, typename TEventArgsT>
    class WeakEventListener
    {
    public:
        WeakEventListener(TInstance const& instance)
           : instance_(instance)
        {
        }

        using OnEventDelegate = std::function<void (TInstance const& instance, TSource const&, TEventArgsT const&)>;
        void OnEventAction(OnEventDelegate handler)
        {
            onEvent_ = std::move(handler);
        }

        using OnDetachDelegate = std::function<void (WeakEventListener const&)>;
        void OnDetachAction(OnDetachDelegate handler)
        {
            onDetach_ = std::move(handler);
        }

        void OnEvent(TSource const& source, TEventArgsT const& eventArgs)
        {
            if (auto const instance = instance_.get())
            {
                if(onEvent_)
                {
                    onEvent_(instance, source, eventArgs);
                }
            }
            else
            {
                Detach();
            }
        }

        void Detach()
        {
            if(onDetach_)
            {
                onDetach_(*this);
                onDetach_ = OnDetachDelegate{};
            }
        }

        bool IsAlive() const
        {
            return instance_.get() != nullptr;
        }

    private:
        winrt::weak_ref<TInstance> instance_;
        OnEventDelegate onEvent_;
        OnDetachDelegate onDetach_;
    };
}
