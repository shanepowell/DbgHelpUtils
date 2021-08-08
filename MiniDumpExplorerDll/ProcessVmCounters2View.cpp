#include "pch.h"
#include "ProcessVmCounters2View.h"
#if __has_include("ProcessVmCounters2View.g.cpp")
#include "ProcessVmCounters2View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ProcessVmCounters2View::ProcessVmCounters2View()
    {
        InitializeComponent();
    }

    IMiniDumpProcessVmCounters2 ProcessVmCounters2View::VmCounters() const
    {
        return vm_counters_;
    }

    void ProcessVmCounters2View::VmCounters(IMiniDumpProcessVmCounters2 const value)
    {
        vm_counters_ = value;
    }
}
