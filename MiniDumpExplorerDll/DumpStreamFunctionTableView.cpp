#include "pch.h"
#include "DumpStreamFunctionTableView.h"
#if __has_include("DumpStreamFunctionTableView.g.cpp")
#include "DumpStreamFunctionTableView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamFunctionTableView::DumpStreamFunctionTableView()
    {
        InitializeComponent();
    }
}
