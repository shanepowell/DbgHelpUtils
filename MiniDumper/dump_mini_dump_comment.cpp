#include "dump_mini_dump_comment.h"

#include <iostream>

#include "DbgHelpUtils/comment_stream_a.h"
#include "DbgHelpUtils/comment_stream_w.h"
#include "DbgHelpUtils/mini_dump.h"

using namespace std;
using namespace dlg_help_utils;

void dump_mini_dump_comment_w_stream_data(mini_dump const& mini_dump, size_t const index)
{
    comment_stream_w const comment{mini_dump, index};

    if (!comment.found())
    {
        wcout << L"CommentStreamW not found!\n";
        return;
    }

    wcout << comment.comment() << L'\n';
    wcout << L'\n';
}

void dump_mini_dump_comment_a_stream_data(mini_dump const& mini_dump, size_t const index)
{
    comment_stream_a const comment{mini_dump, index};

    if (!comment.found())
    {
        wcout << L"CommentStreamA not found!\n";
        return;
    }

    wcout << comment.comment() << L'\n';
    wcout << L'\n';
}
