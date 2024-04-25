#include "unloaded_module_list_stream.h"

#include "mini_dump.h"
#include "module_match.h"

namespace dlg_help_utils
{
    unloaded_module_list_stream::unloaded_module_list_stream(mini_dump const& dump, size_t index)
        : dump_{&dump}
        , module_list_{get_unloaded_module_list(dump, index)}
        , index_{index}
        , found_{module_list_ != nullptr}
        , is_valid_{found_ && module_list_->SizeOfHeader == sizeof(MINIDUMP_UNLOADED_MODULE_LIST) && module_list_->SizeOfEntry == sizeof(MINIDUMP_UNLOADED_MODULE)}
    {
    }

    stream_unloaded_module const* unloaded_module_list_stream::find_module(uint64_t const address) const
    {
        if (!is_valid()) return nullptr;

        for (auto const& module : list())
        {
            if (module->BaseOfImage >= address && address < module->BaseOfImage + module->SizeOfImage)
            {
                return &module;
            }
        }

        return nullptr;
    }

    stream_unloaded_module const* unloaded_module_list_stream::find_module(std::wstring_view const& module_name) const
    {
        if (!is_valid()) return nullptr;

        for (auto const& module : list())
        {
            if (module_match::module_name_match(module, module_name))
            {
                return &module;
            }
        }

        return nullptr;
    }

    MINIDUMP_UNLOADED_MODULE_LIST const* unloaded_module_list_stream::get_unloaded_module_list(mini_dump const& dump, size_t& index)
    {
        auto const* entry = dump.find_stream_type(UnloadedModuleListStream, index);
        if (entry == nullptr)
        {
            return nullptr;
        }

        return static_cast<MINIDUMP_UNLOADED_MODULE_LIST const*>(dump.rva32(entry->Location));
    }

    MINIDUMP_UNLOADED_MODULE const* unloaded_module_list_stream::get_unloaded_module() const
    {
        if (!is_valid_)
        {
            return nullptr;
        }

        return reinterpret_cast<MINIDUMP_UNLOADED_MODULE const*>(reinterpret_cast<uint8_t const*>(module_list_) + sizeof(MINIDUMP_UNLOADED_MODULE_LIST));
    }

    std::vector<stream_unloaded_module> unloaded_module_list_stream::build_modules() const
    {
        std::vector<stream_unloaded_module> rv;
        if (!is_valid()) return rv;

        rv.reserve(module_list_->NumberOfEntries);
        for (size_t index = 0; index < module_list_->NumberOfEntries; ++index)
        {
            rv.emplace_back(*dump_, list_[index]);
        }

        return rv;
    }
}
