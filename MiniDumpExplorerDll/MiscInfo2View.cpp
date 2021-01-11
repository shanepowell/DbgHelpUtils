#include "pch.h"
#include "MiscInfo2View.h"
#if __has_include("MiscInfo2View.g.cpp")
#include "MiscInfo2View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiscInfo2View::MiscInfo2View()
    {
        InitializeComponent();
    }

    IDumpFileStreamMiscInfo MiscInfo2View::Stream() const
    {
        return stream_;
    }

    void MiscInfo2View::Stream(IDumpFileStreamMiscInfo value)
    {
        stream_ = value;
    }
}
