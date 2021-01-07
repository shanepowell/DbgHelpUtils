#include "pch.h"
#include "DumpStreamSystemInfoView.h"
#if __has_include("DumpStreamSystemInfoView.g.cpp")
#include "DumpStreamSystemInfoView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamSystemInfoView::DumpStreamSystemInfoView(IDumpFileStreamSystemInfo stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamSystemInfo DumpStreamSystemInfoView::Stream() const
    {
        return stream_;
    }
}
