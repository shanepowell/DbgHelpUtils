#include "pch.h"
#include "DumpHeader.h"
#if __has_include("DumpHeader.g.cpp")
#include "DumpHeader.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpHeader::DumpHeader()
    {
        InitializeComponent();
    }
}
