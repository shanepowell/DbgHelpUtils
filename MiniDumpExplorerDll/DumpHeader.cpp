#include "pch.h"
#include "DumpHeader.h"
#if __has_include("DumpHeader.g.cpp")
#include "DumpHeader.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DumpHeader::DumpHeader(IDumpFileHeader header)
        : header_{header}
    {
        InitializeComponent();
    }

    IDumpFileHeader DumpHeader::Header() const
    {
        return header_;
    }

    hstring DumpHeader::HeaderType() const
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

    bool DumpHeader::IsKernelDumpFile() const
    {
        return header_.Type() == DumpFileType::KernelModeDumpX86 || header_.Type() == DumpFileType::KernelModeDumpX64;
    }

    bool DumpHeader::IsUserDumpFile() const
    {
        return header_.Type() == DumpFileType::UserModeDump;
    }

    bool DumpHeader::IsInvalidDumpFile() const
    {
        return header_.Type() == DumpFileType::Invalid;
    }

    bool DumpHeader::IsUserOrInvalidDumpFile() const
    {
        return IsUserDumpFile() || IsInvalidDumpFile();
    }
}
