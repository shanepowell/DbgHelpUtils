#include "pch.h"
#include "DumpStreamIptTraceView.h"
#if __has_include("DumpStreamIptTraceView.g.cpp")
#include "DumpStreamIptTraceView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamIptTraceView::DumpStreamIptTraceView()
    {
        InitializeComponent();
    }
}
