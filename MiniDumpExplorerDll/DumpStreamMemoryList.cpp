#include "pch.h"
#include "DumpStreamMemoryList.h"
#if __has_include("DumpStreamMemoryList.g.cpp")
#include "DumpStreamMemoryList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemoryList::DumpStreamMemoryList()
    {
        InitializeComponent();
    }
}
