#include "pch.h"
#include "DumpStreamThreadList.h"
#if __has_include("DumpStreamThreadList.g.cpp")
#include "DumpStreamThreadList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadList::DumpStreamThreadList()
    {
        InitializeComponent();
    }
}
