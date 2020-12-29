#include "pch.h"
#include "DumpStreamMemory64List.h"
#if __has_include("DumpStreamMemory64List.g.cpp")
#include "DumpStreamMemory64List.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMemory64List::DumpStreamMemory64List()
    {
        InitializeComponent();
    }
}
