#include "pch.h"
#include "DumpStreamToken.h"
#if __has_include("DumpStreamToken.g.cpp")
#include "DumpStreamToken.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamToken::DumpStreamToken()
    {
        InitializeComponent();
    }
}
