#include "pch.h"
#include "DumpStreamUnused.h"
#if __has_include("DumpStreamUnused.g.cpp")
#include "DumpStreamUnused.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnused::DumpStreamUnused()
    {
        InitializeComponent();
    }
}
