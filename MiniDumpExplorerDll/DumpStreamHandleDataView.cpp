#include "pch.h"
#include "DumpStreamHandleDataView.h"
#if __has_include("DumpStreamHandleDataView.g.cpp")
#include "DumpStreamHandleDataView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamHandleDataView::DumpStreamHandleDataView()
    {
        InitializeComponent();
    }
}
