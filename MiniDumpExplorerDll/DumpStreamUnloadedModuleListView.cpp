#include "pch.h"
#include "DumpStreamUnloadedModuleListView.h"
#if __has_include("DumpStreamUnloadedModuleListView.g.cpp")
#include "DumpStreamUnloadedModuleListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnloadedModuleListView::DumpStreamUnloadedModuleListView()
    {
        InitializeComponent();
    }
}
