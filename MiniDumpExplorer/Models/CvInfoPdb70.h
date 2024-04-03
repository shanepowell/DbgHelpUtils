#pragma once

#include "CvInfoPdb70.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/cv_info_pdb70.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct CvInfoPdb70 : CvInfoPdb70T<CvInfoPdb70>, GlobalOptionsNotifyPropertyChangedBase<CvInfoPdb70>
    {
        CvInfoPdb70();

        bool IsValid() const { return cv_info_pdb70_.is_valid(); }
        GUID Signature() const { return cv_info_pdb70_.get_signature(); }
        uint32_t Age() const { return cv_info_pdb70_.get_age(); }
        hstring PdbFileName() const { return pdb_file_name_; }

        void Set(dlg_help_utils::cv_info_pdb70 cv_info_pdb70);

    private:
        dlg_help_utils::cv_info_pdb70 cv_info_pdb70_;
        hstring pdb_file_name_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct CvInfoPdb70 : CvInfoPdb70T<CvInfoPdb70, implementation::CvInfoPdb70>
    {
    };
}
