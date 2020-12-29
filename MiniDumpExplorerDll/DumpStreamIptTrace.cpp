#include "pch.h"
#include "DumpStreamIptTrace.h"
#if __has_include("DumpStreamIptTrace.g.cpp")
#include "DumpStreamIptTrace.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamIptTrace::DumpStreamIptTrace()
    {
        InitializeComponent();
    }
}
