#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <string>

namespace dlg_help_utils
{
    class cv_info_pdb70
    {
    public:
        cv_info_pdb70() = default;
        explicit cv_info_pdb70(void const* data, size_t length);

        [[nodiscard]] bool is_valid() const { return cv_ != nullptr && cv_->CvSignature == CV_SIGNATURE_RSDS; }
        [[nodiscard]] GUID const& get_signature() const { return cv_->Signature; }
        [[nodiscard]] DWORD get_age() const { return cv_->Age; }
        [[nodiscard]] std::wstring const& get_pdb_file_name() const { return pdb_file_name_; }

    private:
        // ReSharper disable CppInconsistentNaming
        // ReSharper disable once IdentifierTypo
        // ReSharper disable once CommentTypo
        static const DWORD CV_SIGNATURE_RSDS{0x53445352}; // 'SDSR'

        struct CV_INFO_PDB70
        {
            DWORD CvSignature;
            GUID Signature;
            DWORD Age;
            BYTE PdbFileName[1];
        };

        // ReSharper restore CppInconsistentNaming

        CV_INFO_PDB70 const* cv_{nullptr};
        std::wstring pdb_file_name_;
    };
}
