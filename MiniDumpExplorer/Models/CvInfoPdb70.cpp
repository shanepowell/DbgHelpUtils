#include "pch.h"
#include "CvInfoPdb70.h"

#if __has_include("CvInfoPdb70.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "CvInfoPdb70.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    CvInfoPdb70::CvInfoPdb70()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Age",
            }, { })
    {
    }

    void CvInfoPdb70::Set(dlg_help_utils::cv_info_pdb70 cv_info_pdb70)
    {
        cv_info_pdb70_ = std::move(cv_info_pdb70);
        pdb_file_name_ = cv_info_pdb70_.get_pdb_file_name();
    }
}
