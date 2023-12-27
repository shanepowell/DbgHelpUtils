#include "pch.h"
#include "SuspendedHandlers.h"

#include "DbgHelpUtils/exit_scope.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace
{
    std::unordered_map<DependencyObject, std::unordered_map<DependencyProperty, int>> _suspendedHandlers;


    void SuspendHandler(DependencyObject const& obj, DependencyProperty const& dependencyProperty, bool const incrementSuspensionCount)
    {
        if (auto const suspendedHandlersIt = _suspendedHandlers.find(obj);
            suspendedHandlersIt != _suspendedHandlers.end())
        {
            auto& suspensions = suspendedHandlersIt->second;

            if (incrementSuspensionCount)
            {
                if (auto const suspensionsIt = suspensions.find(dependencyProperty);
                    suspensionsIt != suspensions.end())
                {
                    ++suspensionsIt->second;
                }
                else
                {
                    suspensions.insert(std::make_pair(dependencyProperty, 1));
                }
            }
            else
            {
                auto const suspensionsIt = suspensions.find(dependencyProperty);
                assert(suspensionsIt != suspensions.end());
                if (suspensionsIt->second == 1)
                {
                    suspensions.erase(suspensionsIt);
                }
                else
                {
                    --suspensionsIt->second;
                }

                if (suspensions.size() == 0)
                {
                    _suspendedHandlers.erase(suspendedHandlersIt);
                }
            }
        }
        else
        {
            assert(incrementSuspensionCount);
            std::unordered_map<DependencyProperty, int> suspensions;
            suspensions.insert(std::make_pair(dependencyProperty, 1));
            _suspendedHandlers.insert(std::make_pair(obj, std::move(suspensions)));
        }
    }
}

bool SuspendedHandlers::IsHandlerSuspended(DependencyObject const& dependencyObject, DependencyProperty const& dependencyProperty)
{
    auto const it = _suspendedHandlers.find(dependencyObject);
    if(it == _suspendedHandlers.end())
    {
        return false;
    }

    return it->second.contains(dependencyProperty);
}

void SuspendedHandlers::SetValueNoCallback(DependencyObject const& obj, DependencyProperty const& dependencyProperty, winrt::Windows::Foundation::IInspectable const& value)
{
    SuspendHandler(obj, dependencyProperty, true);
    auto const handle = dlg_help_utils::make_scope_exit([&obj, &dependencyProperty]
        {
            SuspendHandler(obj, dependencyProperty, false);
        });

    obj.SetValue(dependencyProperty, value);
}
