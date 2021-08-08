#include "pch.h"
#include "ModuleView.h"
#if __has_include("ModuleView.g.cpp")
#include "ModuleView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ModuleView::ModuleView()
    {
        InitializeComponent();
    }

    IMiniDumpModule ModuleView::Module() const
    {
        return module_;
    }

    void ModuleView::Module(IMiniDumpModule const value)
    {
        module_ = value;
    }

    hstring ModuleView::Test() const
    {
        return test_;
    }

    void ModuleView::Test(hstring const value)
    {
        test_ = value;
    }
}
