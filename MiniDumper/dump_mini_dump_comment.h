#pragma once

namespace dlg_help_utils
{
    class mini_dump;
}

void dump_mini_dump_comment_w_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
void dump_mini_dump_comment_a_stream_data(dlg_help_utils::mini_dump const& mini_dump, size_t index);
