#include "pch.h"
#include "DumpStreamThreadNames.h"
#if __has_include("DumpStreamThreadNames.g.cpp")
#include "DumpStreamThreadNames.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamThreadNames::DumpStreamThreadNames()
    {
        InitializeComponent();
    }
}
