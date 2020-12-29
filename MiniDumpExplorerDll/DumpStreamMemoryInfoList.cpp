#include "pch.h"
#include "DumpStreamMemoryInfoList.h"
#if __has_include("DumpStreamMemoryInfoList.g.cpp")
#include "DumpStreamMemoryInfoList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemoryInfoList::DumpStreamMemoryInfoList()
    {
        InitializeComponent();
    }
}
