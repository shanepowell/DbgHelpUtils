#include "pch.h"
#include "DumpStreamHandleOperationList.h"
#if __has_include("DumpStreamHandleOperationList.g.cpp")
#include "DumpStreamHandleOperationList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamHandleOperationList::DumpStreamHandleOperationList()
    {
        InitializeComponent();
    }
}
