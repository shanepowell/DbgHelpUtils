#include "pch.h"
#include "DumpStreamSystemMemoryInfo.h"
#if __has_include("DumpStreamSystemMemoryInfo.g.cpp")
#include "DumpStreamSystemMemoryInfo.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamSystemMemoryInfo::DumpStreamSystemMemoryInfo()
    {
        InitializeComponent();
    }
}
