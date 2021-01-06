#include "pch.h"
#include "DumpStreamSystemMemoryInfoView.h"
#if __has_include("DumpStreamSystemMemoryInfoView.g.cpp")
#include "DumpStreamSystemMemoryInfoView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamSystemMemoryInfoView::DumpStreamSystemMemoryInfoView()
    {
        InitializeComponent();
    }
}
