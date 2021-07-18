#include "module_list_stream.h"

#include "mini_dump.h"
#include "module_match.h"

namespace dlg_help_utils
{
    module_list_stream::module_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(ModuleListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        module_list_ = static_cast<MINIDUMP_MODULE_LIST const*>(dump.rva32(entry->Location));
        found_ = true;
    }

    std::experimental::generator<stream_module> module_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        if (!found()) co_return;

        for (size_t index = 0; index < module_list_->NumberOfModules; ++index)
        {
            co_yield stream_module{dump_, module_list_->Modules[index]};
        }
    }

    std::optional<stream_module> module_list_stream::find_module(uint64_t const address) const
    {
        if (!found()) return std::nullopt;

        for (auto&& module : list())
        {
            if (address >= module->BaseOfImage && address < module->BaseOfImage + module->SizeOfImage)
            {
                return module;
            }
        }

        return std::nullopt;
    }

    std::optional<stream_module> module_list_stream::find_module(std::wstring_view const& module_name) const
    {
        if (!found()) return std::nullopt;

        for (auto&& module : list())
        {
            if (module_match::module_name_match(module.name(), module_name))
            {
                return module;
            }
        }

        return std::nullopt;
    }
}
