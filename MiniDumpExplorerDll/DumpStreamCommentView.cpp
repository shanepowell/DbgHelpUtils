#include "pch.h"
#include "DumpStreamCommentView.h"
#if __has_include("DumpStreamCommentView.g.cpp")
#include "DumpStreamCommentView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpStreamCommentView::DumpStreamCommentView(IDumpFileStreamComment stream)
        : stream_{stream}
    {
        InitializeComponent();
    }

    IDumpFileStreamComment DumpStreamCommentView::Stream() const
    {
        return stream_;
    }
}
