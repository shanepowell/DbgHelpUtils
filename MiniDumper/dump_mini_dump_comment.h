#pragma once

#include <ostream>

namespace dlg_help_utils
{
    class mini_dump;
}

void dump_mini_dump_comment_w_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_comment_a_stream_data(std::wostream& log, dlg_help_utils::mini_dump const& mini_dump, size_t index);
