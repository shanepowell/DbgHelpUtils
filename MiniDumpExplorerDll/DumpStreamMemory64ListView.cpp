#include "pch.h"
#include "DumpStreamMemory64ListView.h"
#if __has_include("DumpStreamMemory64ListView.g.cpp")
#include "DumpStreamMemory64ListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemory64ListView::DumpStreamMemory64ListView()
    {
        InitializeComponent();
    }
}
