#include "pch.h"
#include "MiscInfo4View.h"
#if __has_include("MiscInfo4View.g.cpp")
#include "MiscInfo4View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiscInfo4View::MiscInfo4View()
    {
        InitializeComponent();
    }

    IDumpFileStreamMiscInfo MiscInfo4View::Stream() const
    {
        return stream_;
    }

    void MiscInfo4View::Stream(IDumpFileStreamMiscInfo value)
    {
        stream_ = value;
    }
}
