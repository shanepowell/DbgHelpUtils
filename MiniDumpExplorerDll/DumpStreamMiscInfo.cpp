#include "pch.h"
#include "DumpStreamMiscInfo.h"
#if __has_include("DumpStreamMiscInfo.g.cpp")
#include "DumpStreamMiscInfo.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamMiscInfo::DumpStreamMiscInfo()
    {
        InitializeComponent();
    }
}
