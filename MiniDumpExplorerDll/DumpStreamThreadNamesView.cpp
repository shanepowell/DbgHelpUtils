#include "pch.h"
#include "DumpStreamThreadNamesView.h"
#if __has_include("DumpStreamThreadNamesView.g.cpp")
#include "DumpStreamThreadNamesView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadNamesView::DumpStreamThreadNamesView()
    {
        InitializeComponent();
    }
}
