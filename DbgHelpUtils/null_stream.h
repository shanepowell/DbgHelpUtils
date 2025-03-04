#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <ostream>

class null_stream : public std::wostream
{
private:
    class null_buffer : public std::wstreambuf
    {
    public:
        int_type overflow( int_type const ch ) override
        {
            return ch;
        }

        // ReSharper disable once IdentifierTypo
        std::streamsize xsputn( [[maybe_unused]] const char_type* s, std::streamsize const count ) override
        {
            return count;
        }
    } nb_;

public:
    null_stream()
        : std::wostream{&nb_}
    {
    }
};
