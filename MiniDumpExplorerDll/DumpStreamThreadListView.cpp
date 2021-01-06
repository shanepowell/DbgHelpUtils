#include "pch.h"
#include "DumpStreamThreadListView.h"
#if __has_include("DumpStreamThreadListView.g.cpp")
#include "DumpStreamThreadListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadListView::DumpStreamThreadListView()
    {
        InitializeComponent();
    }
}
