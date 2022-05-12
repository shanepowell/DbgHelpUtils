#pragma once

namespace dlg_help_utils
{
    template<typename ... Ts>
    struct overload : Ts ... { 
        using Ts::operator() ...;
    };

    template<class... Ts> overload(Ts...) -> overload<Ts...>;
}