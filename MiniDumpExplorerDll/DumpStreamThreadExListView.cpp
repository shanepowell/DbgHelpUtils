#include "pch.h"
#include "DumpStreamThreadExListView.h"
#if __has_include("DumpStreamThreadExListView.g.cpp")
#include "DumpStreamThreadExListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadExListView::DumpStreamThreadExListView()
    {
        InitializeComponent();
    }
}
