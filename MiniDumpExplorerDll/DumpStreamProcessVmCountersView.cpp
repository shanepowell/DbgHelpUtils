#include "pch.h"
#include "DumpStreamProcessVmCountersView.h"
#if __has_include("DumpStreamProcessVmCountersView.g.cpp")
#include "DumpStreamProcessVmCountersView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamProcessVmCountersView::DumpStreamProcessVmCountersView(IDumpFileStreamProcessVmCounters stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamProcessVmCounters DumpStreamProcessVmCountersView::Stream() const
    {
        return stream_;
    }
}
