#pragma once

namespace dlg_help_utils
{
    class cv_info_pdb70;
}

namespace MiniDumpExplorerApp
{
    struct CvInfoPdb70 : winrt::implements<CvInfoPdb70, winrt::MiniDumpExplorer::ICvInfoPdb70>
    {
        CvInfoPdb70(dlg_help_utils::cv_info_pdb70 const& pdb);

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] winrt::guid Signature() const;
        [[nodiscard]] uint32_t Age() const;
        [[nodiscard]] winrt::hstring PdbFileName() const;

    private:
        dlg_help_utils::cv_info_pdb70 const& pdb_;
        winrt::hstring pdb_file_name_;
    };
}
