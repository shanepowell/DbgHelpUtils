#include "pch.h"
#include "MiscInfo5View.h"
#if __has_include("MiscInfo5View.g.cpp")
#include "MiscInfo5View.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiscInfo5View::MiscInfo5View()
    {
        InitializeComponent();
    }

    IDumpFileStreamMiscInfo MiscInfo5View::Stream() const
    {
        return stream_;
    }

    void MiscInfo5View::Stream(IDumpFileStreamMiscInfo const value)
    {
        stream_ = value;
    }
}
