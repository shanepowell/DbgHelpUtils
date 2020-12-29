#include "pch.h"
#include "DumpStreamUnsupported.h"
#if __has_include("DumpStreamUnsupported.g.cpp")
#include "DumpStreamUnsupported.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamUnsupported::DumpStreamUnsupported()
    {
        InitializeComponent();
    }
}
