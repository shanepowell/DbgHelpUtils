#include "pch.h"
#include "DumpStreamThreadInfoListView.h"
#if __has_include("DumpStreamThreadInfoListView.g.cpp")
#include "DumpStreamThreadInfoListView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadInfoListView::DumpStreamThreadInfoListView()
    {
        InitializeComponent();
    }
}
