#include "pch.h"
#include "DumpStreamModuleListView.h"
#if __has_include("DumpStreamModuleListView.g.cpp")
#include "DumpStreamModuleListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamModuleListView::DumpStreamModuleListView()
    {
        InitializeComponent();
    }
}
