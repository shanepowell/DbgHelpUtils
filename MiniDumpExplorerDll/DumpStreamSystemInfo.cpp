#include "pch.h"
#include "DumpStreamSystemInfo.h"
#if __has_include("DumpStreamSystemInfo.g.cpp")
#include "DumpStreamSystemInfo.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamSystemInfo::DumpStreamSystemInfo()
    {
        InitializeComponent();
    }
}
