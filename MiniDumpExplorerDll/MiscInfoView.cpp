#include "pch.h"
#include "MiscInfoView.h"
#if __has_include("MiscInfoView.g.cpp")
#include "MiscInfoView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiscInfoView::MiscInfoView()
    {
        InitializeComponent();
    }

    IDumpFileStreamMiscInfo MiscInfoView::Stream() const
    {
        return stream_;
    }

    void MiscInfoView::Stream(IDumpFileStreamMiscInfo const value)
    {
        stream_ = value;
    }
}
