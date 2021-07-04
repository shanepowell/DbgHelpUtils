#include "pch.h"
#include "VersionInfoView.h"
#if __has_include("VersionInfoView.g.cpp")
#include "VersionInfoView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    VersionInfoView::VersionInfoView()
    {
        InitializeComponent();
    }

    IVsFixedFileInfo VersionInfoView::VersionInfo()
    {
        return version_info_;
    }

    void VersionInfoView::VersionInfo(IVsFixedFileInfo value)
    {
        version_info_ = value;
    }
}
