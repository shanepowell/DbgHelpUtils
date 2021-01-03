#include "pch.h"
#include "DumpStreamException.h"
#if __has_include("DumpStreamException.g.cpp")
#include "DumpStreamException.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamException::DumpStreamException(IDumpFileStreamException stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamException DumpStreamException::Stream() const
    {
        return stream_;
    }
}
