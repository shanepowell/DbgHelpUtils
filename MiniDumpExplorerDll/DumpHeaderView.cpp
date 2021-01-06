#include "pch.h"
#include "DumpHeaderView.h"
#include "DumpHeaderView.h"
#if __has_include("DumpHeaderView.g.cpp")
#include "DumpHeaderView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpHeaderView::DumpHeaderView(IDumpFileHeader header)
        : header_{header}
    {
        InitializeComponent();
    }

    IDumpFileHeader DumpHeaderView::Header() const
    {
        return header_;
    }

    hstring DumpHeaderView::HeaderType() const
    {
        switch (header_.Type())
        {
        default:
        case DumpFileType::Invalid:
             return L"Invalid Dump";

        case DumpFileType::UserModeDump:
             return L"User Dump";

        case DumpFileType::KernelModeDumpX86:
            return L"X86 Kernel Memory Dump";

        case DumpFileType::KernelModeDumpX64:
            return L"X64 Kernel Memory Dump";
        }
    }

    bool DumpHeaderView::IsKernelDumpFile() const
    {
        return header_.Type() == DumpFileType::KernelModeDumpX86 || header_.Type() == DumpFileType::KernelModeDumpX64;
    }

    bool DumpHeaderView::IsUserDumpFile() const
    {
        return header_.Type() == DumpFileType::UserModeDump;
    }

    bool DumpHeaderView::IsInvalidDumpFile() const
    {
        return header_.Type() == DumpFileType::Invalid;
    }

    bool DumpHeaderView::IsUserOrInvalidDumpFile() const
    {
        return IsUserDumpFile() || IsInvalidDumpFile();
    }
}
