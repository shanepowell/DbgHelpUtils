#include "pch.h"
#include "DumpStreamComment.h"
#if __has_include("DumpStreamComment.g.cpp")
#include "DumpStreamComment.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamComment::DumpStreamComment(IDumpFileStreamComment stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamComment DumpStreamComment::Stream() const
    {
        return stream_;
    }
}
