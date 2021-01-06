#include "pch.h"
#include "DumpStreamTokenView.h"
#if __has_include("DumpStreamTokenView.g.cpp")
#include "DumpStreamTokenView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamTokenView::DumpStreamTokenView()
    {
        InitializeComponent();
    }
}
