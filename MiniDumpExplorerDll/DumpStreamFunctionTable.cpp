#include "pch.h"
#include "DumpStreamFunctionTable.h"
#if __has_include("DumpStreamFunctionTable.g.cpp")
#include "DumpStreamFunctionTable.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamFunctionTable::DumpStreamFunctionTable()
    {
        InitializeComponent();
    }
}
