#include "module_list_stream.h"

#include "mini_dump.h"
#include "module_match.h"

namespace dlg_help_utils
{
    module_list_stream::module_list_stream(mini_dump const& dump, size_t index)
        : dump_{dump}
        , module_list_{get_module_list(dump, index)}
        , index_{index}
        , found_{module_list_ != nullptr}
    {
    }

    stream_module const* module_list_stream::find_module(uint64_t const address) const
    {
        if (!found()) return nullptr;

        for (auto const& module : list())
        {
            if (address >= module->BaseOfImage && address < module->BaseOfImage + module->SizeOfImage)
            {
                return &module;
            }
        }

        return nullptr;
    }

    stream_module const* module_list_stream::find_module(std::wstring_view const& module_name) const
    {
        if (!found()) return nullptr;

        for (auto const& module : list())
        {
            if (module_match::module_name_match(module, module_name))
            {
                return &module;
            }
        }

        return nullptr;
    }

    MINIDUMP_MODULE_LIST const* module_list_stream::get_module_list(mini_dump const& dump, size_t& index)
    {
        auto const* entry = dump.find_stream_type(ModuleListStream, index);
        if (entry == nullptr)
        {
            return nullptr;
        }

        return static_cast<MINIDUMP_MODULE_LIST const*>(dump.rva32(entry->Location));
    }

    std::vector<stream_module> module_list_stream::build_modules() const
    {
        std::vector<stream_module> rv;
        if (!found()) return rv;

        rv.reserve(module_list_->NumberOfModules);
        for (size_t index = 0; index < module_list_->NumberOfModules; ++index)
        {
            rv.emplace_back(dump_, module_list_->Modules[index]);
        }

        return rv;
    }
}
