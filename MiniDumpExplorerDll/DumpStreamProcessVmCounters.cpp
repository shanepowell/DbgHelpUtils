#include "pch.h"
#include "DumpStreamProcessVmCounters.h"
#if __has_include("DumpStreamProcessVmCounters.g.cpp")
#include "DumpStreamProcessVmCounters.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamProcessVmCounters::DumpStreamProcessVmCounters()
    {
        InitializeComponent();
    }
}
