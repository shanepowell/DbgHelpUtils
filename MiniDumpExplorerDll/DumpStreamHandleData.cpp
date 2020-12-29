#include "pch.h"
#include "DumpStreamHandleData.h"
#if __has_include("DumpStreamHandleData.g.cpp")
#include "DumpStreamHandleData.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamHandleData::DumpStreamHandleData()
    {
        InitializeComponent();
    }
}
