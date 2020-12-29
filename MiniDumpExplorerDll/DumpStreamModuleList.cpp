#include "pch.h"
#include "DumpStreamModuleList.h"
#if __has_include("DumpStreamModuleList.g.cpp")
#include "DumpStreamModuleList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamModuleList::DumpStreamModuleList()
    {
        InitializeComponent();
    }
}
