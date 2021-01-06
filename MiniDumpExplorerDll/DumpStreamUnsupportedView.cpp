#include "pch.h"
#include "DumpStreamUnsupportedView.h"
#if __has_include("DumpStreamUnsupportedView.g.cpp")
#include "DumpStreamUnsupportedView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnsupportedView::DumpStreamUnsupportedView()
    {
        InitializeComponent();
    }
}
