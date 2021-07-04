#include "pch.h"
#include "PdbView.h"
#if __has_include("PdbView.g.cpp")
#include "PdbView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    PdbView::PdbView()
    {
        InitializeComponent();
    }

    ICvInfoPdb70 PdbView::Pdb() const
    {
        return pdb_;
    }

    void PdbView::Pdb(ICvInfoPdb70 value)
    {
        pdb_ = value;
    }
}
