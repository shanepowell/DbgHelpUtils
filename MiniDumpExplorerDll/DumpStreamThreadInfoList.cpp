#include "pch.h"
#include "DumpStreamThreadInfoList.h"
#if __has_include("DumpStreamThreadInfoList.g.cpp")
#include "DumpStreamThreadInfoList.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadInfoList::DumpStreamThreadInfoList()
    {
        InitializeComponent();
    }
}
