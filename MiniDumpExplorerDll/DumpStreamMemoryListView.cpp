#include "pch.h"
#include "DumpStreamMemoryListView.h"
#if __has_include("DumpStreamMemoryListView.g.cpp")
#include "DumpStreamMemoryListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemoryListView::DumpStreamMemoryListView()
    {
        InitializeComponent();
    }
}
