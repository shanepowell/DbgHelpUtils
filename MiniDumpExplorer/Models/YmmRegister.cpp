#include "pch.h"
#include "YmmRegister.h"

#include "M128A.h"

#if __has_include("YmmRegister.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "YmmRegister.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    YmmRegister::YmmRegister() = default;

    void YmmRegister::Set(dlg_help_utils::xstate_reader::ymm_register const& ymmRegister)
    {
        name_ = hstring{ std::format(L"YMM{}", ymmRegister.index) };
        index_ = ymmRegister.index;
        xmm_ = MiniDumpExplorer::M128A{};
        xmm_.as<M128A>()->Set(*ymmRegister.xmm);
        ymm_ = MiniDumpExplorer::M128A{};
        ymm_.as<M128A>()->Set(*ymmRegister.ymm);
    }
}
