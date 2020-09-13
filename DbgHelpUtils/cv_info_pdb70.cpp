#include "cv_info_pdb70.h"

#include "string_conversation.h"

namespace dlg_help_utils
{
    cv_info_pdb70::cv_info_pdb70(void const* data, size_t const length)
    {
        if (length < sizeof(CV_INFO_PDB70))
        {
            return;
        }

        cv_ = static_cast<CV_INFO_PDB70 const*>(data);
        if (is_valid())
        {
            pdb_file_name_ = string_conversation::acp_to_wstring(std::string_view{
                reinterpret_cast<char const*>(cv_->PdbFileName)
            });
        }
    }
}
