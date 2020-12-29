#include "pch.h"
#include "DumpStreamThreadExList.h"
#if __has_include("DumpStreamThreadExList.g.cpp")
#include "DumpStreamThreadExList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadExList::DumpStreamThreadExList()
    {
        InitializeComponent();
    }
}
