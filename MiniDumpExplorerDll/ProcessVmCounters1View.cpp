#include "pch.h"
#include "ProcessVmCounters1View.h"
#if __has_include("ProcessVmCounters1View.g.cpp")
#include "ProcessVmCounters1View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ProcessVmCounters1View::ProcessVmCounters1View()
    {
        InitializeComponent();
    }

    IMiniDumpProcessVmCounters1 ProcessVmCounters1View::VmCounters() const
    {
        return vm_counters_;
    }

    void ProcessVmCounters1View::VmCounters(IMiniDumpProcessVmCounters1 value)
    {
        vm_counters_ = value;
    }
}
