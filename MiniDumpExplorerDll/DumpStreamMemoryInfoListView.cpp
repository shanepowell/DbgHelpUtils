#include "pch.h"
#include "DumpStreamMemoryInfoListView.h"
#if __has_include("DumpStreamMemoryInfoListView.g.cpp")
#include "DumpStreamMemoryInfoListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemoryInfoListView::DumpStreamMemoryInfoListView()
    {
        InitializeComponent();
    }
}
