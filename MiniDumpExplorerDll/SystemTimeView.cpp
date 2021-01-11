#include "pch.h"
#include "SystemTimeView.h"
#if __has_include("SystemTimeView.g.cpp")
#include "SystemTimeView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    SystemTimeView::SystemTimeView()
    {
        InitializeComponent();
    }

    ISystemTime SystemTimeView::SystemTime()
    {
        return system_time_;
    }

    void SystemTimeView::SystemTime(ISystemTime value)
    {
        system_time_ = value;
    }
}
