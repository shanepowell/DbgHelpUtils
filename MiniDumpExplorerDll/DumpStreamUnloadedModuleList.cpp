#include "pch.h"
#include "DumpStreamUnloadedModuleList.h"
#if __has_include("DumpStreamUnloadedModuleList.g.cpp")
#include "DumpStreamUnloadedModuleList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnloadedModuleList::DumpStreamUnloadedModuleList()
    {
        InitializeComponent();
    }
}
