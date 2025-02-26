#pragma once

#include "MiniDumpPage.xaml.h"

#include "Utility/logger.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    template<typename T>
    struct MiniDumpPageBase
    {
    public:
        MiniDumpPageBase() = default;
        virtual ~MiniDumpPageBase() = default;

        MiniDumpPageBase(MiniDumpPageBase const&) = default;
        MiniDumpPageBase(MiniDumpPageBase &&) = default;

        MiniDumpPageBase& operator=(MiniDumpPageBase const&) = default;
        MiniDumpPageBase& operator=(MiniDumpPageBase &&) = default;

        // ReSharper disable once CppHiddenFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            auto parameters = e.Parameter().as<MiniDumpExplorer::MiniDumpPageParameters>();
            if(auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
                miniDumpPage->MiniDumpOpened())
            {
                MiniDumpLoaded(parameters);
            }
            else
            {
                auto event = std::make_shared<event_token>();

                *event = miniDumpPage->MiniDumpLoaded([weakThis{ Self()->get_weak() }, this, parameters, event]([[maybe_unused]] auto const& sender, [[maybe_unused]] auto const& args)
                {
                    if (auto strongThis{ weakThis.get() }; strongThis)
                    {
                        try
                        {
                            MiniDumpLoaded(parameters);
                        }
                        catch(...)
                        {
                            logger::Log().HandleUnknownException();
                        }

                        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
                        miniDumpPage->MiniDumpLoaded(*event);
                    }
                });
            }
        }

    protected:
        virtual void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) = 0;

        T* Self() noexcept
        {
            return static_cast<T*>(this);
        }
    };
}