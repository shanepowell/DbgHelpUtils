#include "pch.h"
#include "DumpStreamHandleOperationListView.h"
#if __has_include("DumpStreamHandleOperationListView.g.cpp")
#include "DumpStreamHandleOperationListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamHandleOperationListView::DumpStreamHandleOperationListView()
    {
        InitializeComponent();
    }
}
