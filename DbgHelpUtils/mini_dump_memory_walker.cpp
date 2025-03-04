#include "mini_dump_memory_walker.h"

#include <filesystem>
#include <format>
#include <string>

#include "function_table_stream.h"
#include "hex_dump.h"
#include "locale_number_formatting.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "memory_range.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "range_units.h"
#include "stream_hex_dump.h"
#include "symbol_engine.h"
#include "unloaded_module_list_stream.h"

using namespace std::string_literals;

namespace dlg_help_utils::stream_stack_dump
{
    mini_dump_memory_walker::mini_dump_memory_walker(DWORD64 const stack_start_range
        , void const* stack
        , size_t const stack_size
        , memory_list_stream const& memory_list
        , memory64_list_stream const& memory64_list
        , function_table_stream const& function_table
        , module_list_stream const& module_list
        , unloaded_module_list_stream const& unloaded_module_list
        , pe_file_memory_mapping& pe_file_memory_mappings
        , dbg_help::symbol_engine& symbol_engine)
        : stack_start_range_{ stack_start_range }
        , stack_{ static_cast<uint8_t const*>(stack) }
        , stack_size_{ stack_size }
        , memory_list_{ memory_list }
        , memory64_list_{ memory64_list }
        , function_table_{ function_table }
        , module_list_{ module_list }
        , unloaded_module_list_{ unloaded_module_list }
        , pe_file_memory_mappings_{ pe_file_memory_mappings }
        , symbol_engine_{ symbol_engine }
        , callback_{ symbol_engine.load_callback() }
        , handle_{ dbg_help::symbol_engine::set_walk_callback(*this) }
    {
    }

    // ReSharper disable CppParameterMayBeConst
    bool mini_dump_memory_walker::read_process_memory(DWORD64 const base_address
        , PVOID buffer
        , DWORD const size
        , LPDWORD number_of_bytes_read
        , enable_module_loading_t const enable_module_loading)
    {
        if (!range_utils::validate_range(base_address, size))
        {
            if (callback_.symbol_load_debug())
            {
                callback_.log_stream().log() << std::format(L"read_process_memory {0} - {1} : invalid address range\n", stream_hex_dump::to_hex(base_address), locale_formatting::to_wstring(size));
            }
            return false;
        }

        auto const result = do_read_process_memory(base_address
            , buffer
            , size
            , number_of_bytes_read
            , enable_module_loading);

        if (callback_.symbol_load_debug())
        {
            {
                auto handle = callback_.log_stream();
                handle.log() << std::format(L"read_process_memory {0} - {1} : {2}", stream_hex_dump::to_hex(base_address), locale_formatting::to_wstring(size), result);
                if (result)
                {
                    handle.log() << std::format(L" : bytes read: {}", locale_formatting::to_wstring(*number_of_bytes_read));
                    if (*number_of_bytes_read > size)
                    {
                        handle.log() << L" - error invalid result";
                    }
                    if (*number_of_bytes_read < size)
                    {
                        handle.log() << L" - short read";
                    }
                }
                handle.log() << L'\n';
            }

            if (result)
            {
                if (callback_.symbol_load_debug_memory())
                {
                    auto handle = callback_.log_stream();
                    hex_dump::hex_dump(handle.log(), buffer, *number_of_bytes_read, 5, write_header_t{true}, 16, base_address);
                }
            }
        }

        return result;
    }

    PVOID mini_dump_memory_walker::function_table_access(DWORD64 base_address)
    {
        if (callback_.symbol_load_debug())
        {
            callback_.log_stream().log() << std::format(L"function_table_access {}\n", stream_hex_dump::to_hex(base_address));
        }
        return get_mini_dump_function_table(base_address);
    }

    DWORD64 mini_dump_memory_walker::get_module_base_routine(DWORD64 address)
    {
        if (callback_.symbol_load_debug())
        {
            callback_.log_stream().log() << std::format(L"get_module_base_routine {}\n", stream_hex_dump::to_hex(address));
        }
        if (auto const base_address = get_loaded_module_base_routine(address); base_address != 0) return base_address;
        return get_unloaded_module_base_routine(address);
    }

    DWORD64 mini_dump_memory_walker::translate_address([[maybe_unused]] HANDLE h_thread, LPADDRESS64 lp_address)
    {
        if (lp_address == nullptr) return 0;
        return lp_address->Offset;
    }

    mini_dump_memory_stream mini_dump_memory_walker::get_process_memory_stream(DWORD64 base_address, DWORD64 size, enable_module_loading_t enable_module_loading) const
    {
        return mini_dump_memory_stream
        {
            [this](uint64_t const base_address, uint64_t& size, enable_module_loading_t const enable_module_loading)
            {
                return get_process_memory_range(base_address, size, enable_module_loading);
            }
            , base_address
            , size
            , enable_module_loading
        };
    }

    void mini_dump_memory_walker::release_handle()
    {
        handle_.release();
    }

    void mini_dump_memory_walker::create_handle()
    {
        handle_ = dbg_help::symbol_engine::set_walk_callback(*this);
    }

    void const* mini_dump_memory_walker::get_process_memory_range(DWORD64 const base_address, DWORD64& size, enable_module_loading_t const enable_module_loading) const
    {
        auto max_size = size;
        auto const* memory = get_stack_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory64_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        max_size = size;
        memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            size = max_size;
            return memory;
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            max_size = size;
            memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
            if (memory != nullptr)
            {
                size = max_size;
                return memory;
            }
        }

        return nullptr;
    }

    DWORD64 mini_dump_memory_walker::find_memory_range(DWORD64 const base_address, DWORD64 const element_size, DWORD64 const max_elements, enable_module_loading_t const enable_module_loading) const
    {
        DWORD64 max_size = element_size * max_elements;
        auto const* memory = get_stack_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory64_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return max_size;
        }

        memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return max_size;
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
            if (memory != nullptr)
            {
                return max_size;
            }
        }

        return 0;
    }

    DWORD64 mini_dump_memory_walker::find_memory_range_if(DWORD64 const base_address
        , DWORD64 const element_size
        , DWORD64 const max_elements
        , std::function<bool(void const*)> const& pred
        , enable_module_loading_t const enable_module_loading) const
    {
        DWORD64 max_size = element_size * max_elements;
        auto const* memory = get_stack_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory64_list_memory(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
        if (memory != nullptr)
        {
            return find_max_element_size(memory, element_size, max_size, pred);
        }

        if (enable_module_loading && load_pe_file(base_address))
        {
            memory = get_memory_from_pe_file(base_address, max_size, limit_size_t{false});
            if (memory != nullptr)
            {
                return find_max_element_size(memory, element_size, max_size, pred);
            }
        }

        return 0;
    }

    bool mini_dump_memory_walker::do_read_process_memory(DWORD64 const base_address
        , PVOID buffer
        , DWORD const size
        , LPDWORD number_of_bytes_read
        , enable_module_loading_t const enable_module_loading) const
    {
        if (read_stack_memory(base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        if (memory_list_.found())
        {
            if (read_memory_list_memory(base_address, buffer, size, number_of_bytes_read))
            {
                return true;
            }
        }

        if (memory64_list_.found())
        {
            if (read_memory64_list_memory(base_address, buffer, size, number_of_bytes_read))
            {
                return true;
            }
        }

        if (read_memory_from_pe_file(base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        if (enable_module_loading && load_pe_file(base_address) && read_memory_from_pe_file(
            base_address, buffer, size, number_of_bytes_read))
        {
            return true;
        }

        return false;
    }

    bool mini_dump_memory_walker::read_stack_memory(DWORD64 const base_address
        , PVOID buffer
        , DWORD size
        , LPDWORD number_of_bytes_read) const
    {
        if(stack_size_ == 0) return false;
        uint64_t length = size;
        if (!range_utils::range_union(stack_start_range_, stack_size_, base_address, length)) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);

        memcpy(buffer, stack_ + (base_address - stack_start_range_), *number_of_bytes_read);
        return true;
    }

    bool mini_dump_memory_walker::read_memory_list_memory(DWORD64 base_address
        , PVOID buffer
        , DWORD size
        , LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = memory_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_memory_walker::read_memory64_list_memory(DWORD64 base_address
        , PVOID buffer
        , DWORD size
        , LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = memory64_list_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    bool mini_dump_memory_walker::read_memory_from_pe_file(DWORD64 base_address
        , PVOID buffer
        , DWORD size
        , LPDWORD number_of_bytes_read) const
    {
        DWORD64 length = size;
        auto const* memory = pe_file_memory_mappings_.find_any_address_range(base_address, length);
        if (memory == nullptr) return false;
        *number_of_bytes_read = static_cast<DWORD>(length);
        memcpy(buffer, memory, *number_of_bytes_read);
        return true;
    }

    void const* mini_dump_memory_walker::get_stack_memory(DWORD64 const base_address, DWORD64& size, limit_size_t const limit_size) const
    {
        if(stack_size_ == 0) return nullptr;
        if (DWORD64 length = size; range_utils::range_union(stack_start_range_, stack_size_, base_address, length) && (!limit_size || length == size))
        {
            if(!limit_size)
            {
                size = length;
            }
            return stack_ + (base_address - stack_start_range_);
        }

        return nullptr;
    }

    template<typename T>
    void const* mini_dump_memory_walker::get_memory_range_for_address(T const& memory_list, DWORD64 const base_address, DWORD64& size, limit_size_t const limit_size)
    {
        if (memory_list.found())
        {
            // a memory range can cross boundaries and that is OK...
            auto base = base_address;
            auto size_left = size;
            DWORD64 length = size;
            void const* rv = nullptr;

            if(!limit_size)
            {
                size = 0;
            }

            while(size_left > 0)
            {
                if (auto const* memory = memory_list.find_any_address_range(base, length); memory != nullptr && (!limit_size || length == size))
                {
                    if(!limit_size)
                    {
                        size += length;
                    }

                    size_left -= length;
                    base += length;
                    length = size_left;

                    if(rv == nullptr)
                    {
                        rv = memory;
                    }
                }
                else
                {
                    break;
                }
            }

            return rv;
        }

        return nullptr;
    }

    void const* mini_dump_memory_walker::get_memory_list_memory(DWORD64 const base_address, DWORD64& size, limit_size_t const limit_size) const
    {
        return get_memory_range_for_address(memory_list_, base_address, size, limit_size);
    }

    void const* mini_dump_memory_walker::get_memory64_list_memory(DWORD64 const base_address, DWORD64& size, limit_size_t const limit_size) const
    {
        return get_memory_range_for_address(memory64_list_, base_address, size, limit_size);
    }

    void const* mini_dump_memory_walker::get_memory_from_pe_file(DWORD64 const base_address, DWORD64& size, limit_size_t const limit_size) const
    {
        DWORD64 length = size;
        auto const* memory = pe_file_memory_mappings_.find_any_address_range(base_address, length);
        if (memory == nullptr || (limit_size && length != size)) return nullptr;
        if(!limit_size)
        {
            size = length;
        }
        return memory;
    }

    bool mini_dump_memory_walker::load_pe_file(DWORD64 base_address) const
    {
        if (load_loaded_module(base_address))
        {
            return true;
        }

        return load_unloaded_module(base_address);
    }

    bool mini_dump_memory_walker::load_loaded_module(DWORD64 base_address) const
    {
        auto const module = module_list_.find_module(base_address);
        if (!module)
        {
            return false;
        }

        std::ignore = load_module(*module);

        std::wstring const name{module->name()};
        auto const& image_path = symbol_engine_.get_module_image_path(name);
        if (image_path.empty() || !std::filesystem::exists(image_path) || !std::filesystem::is_regular_file(image_path))
        {
            return false;
        }

        pe_file_memory_mappings_.load_pe_file(image_path, (*module)->BaseOfImage);
        return true;
    }

    bool mini_dump_memory_walker::load_unloaded_module(DWORD64 base_address) const
    {
        auto const module = unloaded_module_list_.find_module(base_address);
        if (!module)
        {
            return false;
        }

        std::ignore = load_module(*module);

        std::wstring const name{module->name()};
        if (auto const & image_path = symbol_engine_.get_module_image_path(name); image_path.empty() && std::filesystem::exists(image_path) && std::filesystem::is_regular_file(image_path))
        {
            pe_file_memory_mappings_.load_pe_file(image_path, (*module)->BaseOfImage);
            return true;
        }

        return false;
    }

    PVOID mini_dump_memory_walker::get_mini_dump_function_table(DWORD64 base_address) const
    {
        if (!function_table_.found() || !function_table_.is_valid()) return nullptr;

        for (auto const& table : function_table_.list())
        {
            if (table->BaseAddress == base_address || base_address >= table->MinimumAddress && base_address <= table->
                MaximumAddress)
            {
                return const_cast<PVOID>(table.raw_function_table());
            }
        }
        return nullptr;
    }

    DWORD64 mini_dump_memory_walker::get_loaded_module_base_routine(DWORD64 address) const
    {
        if (!module_list_.found()) return 0;

        auto const module = module_list_.find_module(address);
        if (!module) return 0;

        std::ignore = load_module(*module);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream().log() << std::format(L"get_module_base_routine {0} is in {1} : {2}\n", stream_hex_dump::to_hex(address), module->name(), stream_hex_dump::to_hex((*module)->BaseOfImage));
        }
        return (*module)->BaseOfImage;
    }

    DWORD64 mini_dump_memory_walker::get_unloaded_module_base_routine(DWORD64 address) const
    {
        if (!unloaded_module_list_.found()) return 0;

        auto const module = unloaded_module_list_.find_module(address);
        if (!module) return 0;

        std::ignore = load_module(*module);

        if (callback_.symbol_load_debug())
        {
            callback_.log_stream().log() << std::format(L"get_module_base_routine {0} is in {1} : {2}\n", stream_hex_dump::to_hex(address), module->name(), stream_hex_dump::to_hex((*module)->BaseOfImage));
        }
        return (*module)->BaseOfImage;
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_memory_walker::find_symbol_info(dbg_help::thread_context_type const type, STACKFRAME_EX const& frame, void const* thread_context) const
    {
        auto const it_module = module_list_.find_module(frame.AddrPC.Offset);
        if (!it_module) return find_unloaded_module_symbol_info(frame.AddrPC.Offset, unloaded_module_list_, symbol_engine_);

        std::ignore = load_module(*it_module);
        return symbol_engine_.address_to_info(type, frame, thread_context);
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_memory_walker::find_symbol_info(
        uint64_t address, module_list_stream const& module_list,
        unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine)
    {
        auto const it_module = module_list.find_module(address);
        if (!it_module) return find_unloaded_module_symbol_info(address, unloaded_module_list, symbol_engine);

        auto const& module{*it_module};

        if (std::wstring const module_name{module.name()}; !symbol_engine.is_module_loaded(module_name))
        {
            symbol_engine.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                      module->CheckSum, module.cv_record(), module->CvRecord.DataSize,
                                      module.misc_record(), module->MiscRecord.DataSize, module->VersionInfo);
        }

        return symbol_engine.address_to_info(address);
    }

    std::optional<dbg_help::symbol_address_info> mini_dump_memory_walker::find_unloaded_module_symbol_info(uint64_t address, unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine)
    {
        if (!unloaded_module_list.found()) return std::nullopt;

        auto const it_module = unloaded_module_list.find_module(address);
        if (!it_module) return std::nullopt;

        auto const& module{*it_module};

        if (std::wstring const module_name{it_module->name()}; !symbol_engine.is_module_loaded(module_name))
        {
            symbol_engine.load_module(module_name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                      module->CheckSum);
        }

        return symbol_engine.address_to_info(address);
    }

    DWORD64 mini_dump_memory_walker::find_max_element_size(void const* memory, DWORD64 element_size, DWORD64 max_size, std::function<bool(void const*)> const& pred)
    {
        auto const element_max = max_size / element_size;
        DWORD64 index = 0;
        for(; index < element_max; ++ index, memory = static_cast<char const*>(memory) + element_size)
        {
            if(pred(memory))
            {
                break;
            }
        }

        return index;
    }

    // ReSharper restore CppParameterMayBeConst
    std::optional<dbg_help::symbol_type_info> mini_dump_memory_walker::get_type_info(std::wstring const& type_name, throw_on_error_t const throw_on_error) const
    {
        if(auto const it = cache_type_.find(type_name); it != cache_type_.end())
        {
            return it->second;
        }

        if(type_name.empty())
        {
            return std::nullopt;
        }

        auto [module_name, specific_type_name] = dbg_help::symbol_engine::parse_type_info(type_name);

        if(module_name.empty())
        {
            // load all loaded modules...
            for (auto const& module : module_list_.list())
            {
                std::ignore = load_module(module, throw_on_error);
            }
        }
        else
        {
            std::ignore = load_module(module_name, throw_on_error);
        }

        auto rv = symbol_engine_.get_type_info(module_name, specific_type_name, throw_on_error);
        cache_type_.insert(std::make_pair(type_name, rv));
        return rv;
    }

    std::optional<dbg_help::symbol_type_info> mini_dump_memory_walker::get_symbol_info(std::wstring const& symbol_name, throw_on_error_t const throw_on_error) const
    {
        auto [module_name, specific_type_name] = dbg_help::symbol_engine::parse_type_info(symbol_name);

        if(specific_type_name.empty())
        {
            return std::nullopt;
        }

        if(module_name.empty())
        {
            // load all loaded modules...
            for (auto const& module : module_list_.list())
            {
                std::ignore = load_module(module, throw_on_error);
            }
        }
        else
        {
            std::ignore = load_module(module_name, throw_on_error);
        }

        return symbol_engine_.get_symbol_info(symbol_name, throw_on_error);
    }

    std::vector<dbg_help::symbol_type_info> mini_dump_memory_walker::module_types(std::wstring const& module_name) const
    {
        std::ignore = load_module(module_name);
        return symbol_engine_.module_types(module_name);
    }

    std::vector<dbg_help::symbol_type_info> mini_dump_memory_walker::symbol_walk(std::wstring const& find_mask) const
    {
        std::optional<ULONG64> module_base;
        auto [module_name, specific_type_name] = dbg_help::symbol_engine::parse_type_info(find_mask);
        if(module_name.empty() || module_name == L"*"s)
        {
            // load all loaded modules...
            for (auto const& module : module_list_.list())
            {
                std::ignore = load_module(module);
            }
        }
        else
        {
            module_base = load_module(module_name);
        }

        return symbol_engine_.symbol_walk(specific_type_name, module_base);
    }

    bool mini_dump_memory_walker::load_module_from_address(DWORD64 const base_address) const
    {
        if(auto const module = module_list_.find_module(base_address); module)
        {
            std::ignore = load_module(*module);
            return true;
        }

        return false;
    }

    generator<memory_range> mini_dump_memory_walker::memory_ranges() const
    {
        if(memory_list_.found())
        {
            for (auto const& range : memory_list_.memory_ranges())
            {
                co_yield range;
            }
        }
        else if(memory64_list_.found())
        {
            for (auto const& range : memory64_list_.memory_ranges())
            {
                co_yield range;
            }
        }
    }

    std::optional<ULONG64> mini_dump_memory_walker::load_module(std::wstring const& module_name, throw_on_error_t const throw_on_error) const
    {
        if(auto const & module = module_list_.find_module(module_name); module)
        {
            return load_module(*module, throw_on_error);
        }
        else
        {
            if(auto const & unloaded_module = unloaded_module_list_.find_module(module_name); unloaded_module)
            {
                return load_module(*unloaded_module, throw_on_error);
            }
        }

        return std::nullopt;
    }

    std::optional<ULONG64> mini_dump_memory_walker::load_module(stream_module const& module, throw_on_error_t const throw_on_error) const
    {
        if (std::wstring const name{module.name()}; !symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                       module->CheckSum, module.cv_record(), module->CvRecord.DataSize,
                                       module.misc_record(), module->MiscRecord.DataSize, module->VersionInfo, throw_on_error);
            return module->BaseOfImage;
        }

        return std::nullopt;
    }

    std::optional<ULONG64> mini_dump_memory_walker::load_module(stream_unloaded_module const& module, throw_on_error_t const throw_on_error) const
    {
        if (std::wstring const name{module.name()}; !symbol_engine_.is_module_loaded(name))
        {
            symbol_engine_.load_module(name, module->BaseOfImage, module->SizeOfImage, module->TimeDateStamp,
                                       module->CheckSum, throw_on_error);
            return module->BaseOfImage;
        }

        return std::nullopt;
    }

}
