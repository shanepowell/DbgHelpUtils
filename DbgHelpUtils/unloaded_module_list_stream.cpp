#include "unloaded_module_list_stream.h"

#include "mini_dump.h"
#include "module_match.h"

namespace dlg_help_utils
{
    unloaded_module_list_stream::unloaded_module_list_stream(mini_dump const& dump, size_t const index)
        : dump_{dump}
          , index_{index}
    {
        auto const* entry = dump.find_stream_type(UnloadedModuleListStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        module_list_ = static_cast<MINIDUMP_UNLOADED_MODULE_LIST const*>(dump.rva32(entry->Location));
        found_ = true;

        is_valid_ = module_list_->SizeOfHeader == sizeof(MINIDUMP_UNLOADED_MODULE_LIST) && module_list_->SizeOfEntry ==
            sizeof(MINIDUMP_UNLOADED_MODULE);

        if (is_valid_)
        {
            list_ = reinterpret_cast<MINIDUMP_UNLOADED_MODULE const*>(reinterpret_cast<uint8_t const*>(module_list_) +
                sizeof(MINIDUMP_UNLOADED_MODULE_LIST));
        }
    }

    std::experimental::generator<stream_unloaded_module> unloaded_module_list_stream::list() const  // NOLINT(bugprone-reserved-identifier)
    {
        if (!is_valid()) co_return;

        for (size_t index = 0; index < module_list_->NumberOfEntries; ++index)
        {
            co_yield stream_unloaded_module{dump_, list_[index]};
        }
    }

    std::optional<stream_unloaded_module> unloaded_module_list_stream::find_module(uint64_t const address) const
    {
        if (!is_valid()) return std::nullopt;

        for (auto&& module : list())
        {
            if (module->BaseOfImage >= address && address < module->BaseOfImage + module->SizeOfImage)
            {
                return module;
            }
        }

        return std::nullopt;
    }

    std::optional<stream_unloaded_module> unloaded_module_list_stream::find_module(std::wstring_view const& module_name) const
    {
        if (!is_valid()) return std::nullopt;

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
