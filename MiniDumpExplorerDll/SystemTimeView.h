#pragma once

#include "SystemTimeView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SystemTimeView : SystemTimeViewT<SystemTimeView>
    {
        SystemTimeView();

        ISystemTime SystemTime();
        void SystemTime(ISystemTime value);

    private:
        ISystemTime system_time_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SystemTimeView : SystemTimeViewT<SystemTimeView, implementation::SystemTimeView>
    {
    };
}
