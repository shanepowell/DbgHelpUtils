#include "pch.h"
#include "CvInfoPdb70.h"

#include "../DbgHelpUtils/cv_info_pdb70.h"

namespace MiniDumpExplorerApp
{
    CvInfoPdb70::CvInfoPdb70(dlg_help_utils::cv_info_pdb70 const& pdb)
        : pdb_{pdb}
    {
        if(pdb_.is_valid())
        {
            pdb_file_name_ = pdb_.get_pdb_file_name();
        }
    }

    bool CvInfoPdb70::IsValid() const
    {
        return pdb_.is_valid();
    }

    winrt::guid CvInfoPdb70::Signature() const
    {
        return {pdb_.get_signature()};
    }

    uint32_t CvInfoPdb70::Age() const
    {
        return pdb_.get_age();
    }

    winrt::hstring CvInfoPdb70::PdbFileName() const
    {
        return pdb_file_name_;
    }
}
