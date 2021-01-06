#include "pch.h"
#include "DumpStreamUnusedView.h"
#if __has_include("DumpStreamUnusedView.g.cpp")
#include "DumpStreamUnusedView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnusedView::DumpStreamUnusedView()
    {
        InitializeComponent();
    }
}
