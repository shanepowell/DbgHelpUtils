#include "pch.h"
#include "DumpStreamException.h"
#if __has_include("DumpStreamException.g.cpp")
#include "DumpStreamException.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamException::DumpStreamException()
    {
        InitializeComponent();
    }
}
