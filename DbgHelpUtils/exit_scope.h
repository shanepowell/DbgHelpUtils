#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <utility>

namespace dlg_help_utils
{
    template <typename F>
    class scope_exit
    {
    public:
        scope_exit(F&& value)  // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
            : value_{std::forward<F>(value)}
        {
        }

        ~scope_exit()
        {
            value_();
        }

        scope_exit(scope_exit const&) = delete;
        scope_exit(scope_exit&&) = delete;

        scope_exit& operator=(scope_exit const&) = delete;
        scope_exit& operator=(scope_exit&&) = delete;

    private:
        F value_;
    };

    template <typename F>
    auto make_scope_exit(F&& value)
    {
        return scope_exit<F>(std::forward<F>(value));
    }
}
