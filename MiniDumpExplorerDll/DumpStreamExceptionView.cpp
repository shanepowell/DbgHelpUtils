#include "pch.h"
#include "DumpStreamExceptionView.h"
#if __has_include("DumpStreamExceptionView.g.cpp")
#include "DumpStreamExceptionView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamExceptionView::DumpStreamExceptionView(IDumpFileStreamException stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamException DumpStreamExceptionView::Stream() const
    {
        return stream_;
    }
}
