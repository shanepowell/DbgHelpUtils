#include "pch.h"
#include "MiscInfo3View.h"
#if __has_include("MiscInfo3View.g.cpp")
#include "MiscInfo3View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiscInfo3View::MiscInfo3View()
    {
        InitializeComponent();
    }

    IDumpFileStreamMiscInfo MiscInfo3View::Stream() const
    {
        return stream_;
    }

    void MiscInfo3View::Stream(IDumpFileStreamMiscInfo value)
    {
        stream_ = value;
    }
}
