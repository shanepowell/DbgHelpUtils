#include "stream_module.h"

#include "mini_dump.h"
#include "string_stream.h"

namespace dlg_help_utils
{
    stream_module::stream_module(mini_dump const& dump, MINIDUMP_MODULE const& module)
        : module_{module}
    {
        name_ = string_stream::to_string(dump, module_.ModuleNameRva);

        if (module_.CvRecord.DataSize > 0)
        {
            cv_record_ = dump.rva32(module_.CvRecord);
            pdb_info_ = cv_info_pdb70{cv_record_, module_.CvRecord.DataSize};
        }

        if (module_.MiscRecord.DataSize > 0)
        {
            misc_record_ = dump.rva32(module_.MiscRecord);
        }
    }
}
