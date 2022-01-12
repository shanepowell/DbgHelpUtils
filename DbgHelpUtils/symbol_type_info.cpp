#include "symbol_type_info.h"

#include <format>

#include "string_compare.h"
#include "symbol_engine.h"
#include "windows_error.h"

using namespace std::string_view_literals;

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info::cache_type_info
    {
    public:
        [[nodiscard]] bool has_name() const{ return has_cached_name_info_; }
        [[nodiscard]] std::optional<std::wstring> const& name() const { return cached_name_info_; }
        void set_name(std::optional<std::wstring> value) { cached_name_info_ = std::move(value); has_cached_name_info_ = true; }

        [[nodiscard]] bool find_field_in_type(std::wstring_view const& field_name, std::optional<std::pair<symbol_type_info, uint64_t>>& rv)
        {
            if(auto const it = cached_find_field_in_type_.find(field_name); it != cached_find_field_in_type_.end())
            {
                rv = it->second;
                return true;
            }

            return false;
        }

        void set_find_field_in_type(std::wstring_view const& field_name, std::optional<std::pair<symbol_type_info, uint64_t>> value)
        {
            cached_find_field_in_type_.insert(std::make_pair(field_name, std::move(value)));
        }

        [[nodiscard]] bool has_children() const { return has_cached_children_; }
        [[nodiscard]] std::vector<symbol_type_info> const& children() const { return cached_children_; }
        void set_children(std::vector<symbol_type_info> value)
        {
            if(has_cached_children_)
            {
                // someone beat us to it...
                return;
            }

            cached_children_ = std::move(value);
            has_cached_children_ = true;
        }

        [[nodiscard]] bool has_type() const{ return has_cached_type_; }
        [[nodiscard]] std::optional<symbol_type_info> const& type() const { return cached_type_; }
        void set_type(std::optional<symbol_type_info> value) { cached_type_ = std::move(value); has_cached_type_ = true; }

        [[nodiscard]] bool has_sym_tag() const{ return has_cached_sym_tag_; }
        [[nodiscard]] std::optional<sym_tag_enum> const& sym_tag() const { return cached_sym_tag_; }
        void set_sym_tag(std::optional<sym_tag_enum> const value) { cached_sym_tag_ = value; has_cached_sym_tag_ = true; }

        [[nodiscard]] bool has_offset() const{ return has_cached_offset_; }
        [[nodiscard]] std::optional<DWORD> const& offset() const { return cached_offset_; }
        void set_offset(std::optional<DWORD> const value) { cached_offset_ = value; has_cached_offset_ = true; }

        [[nodiscard]] bool has_length() const{ return has_cached_length_; }
        [[nodiscard]] std::optional<ULONG64> const& length() const { return cached_length_; }
        void set_length(std::optional<ULONG64> const value) { cached_length_ = value; has_cached_length_ = true; }

    private:
        std::map<std::wstring_view, std::optional<std::pair<symbol_type_info, uint64_t>>> cached_find_field_in_type_;
        bool has_cached_name_info_{false};
        std::optional<std::wstring> cached_name_info_;
        bool has_cached_children_{false};
        std::vector<symbol_type_info> cached_children_;
        bool has_cached_type_{false};
        std::optional<symbol_type_info> cached_type_;
        bool has_cached_sym_tag_{false};
        std::optional<sym_tag_enum> cached_sym_tag_;
        bool has_cached_offset_{false};
        std::optional<DWORD> cached_offset_;
        bool has_cached_length_{false};
        std::optional<ULONG64> cached_length_;
    };


    symbol_type_info::symbol_type_info(DWORD64 const module_base, ULONG const type_index)
    : module_base_{module_base}
    , type_index_{type_index}
    , cache_info_{std::make_shared<cache_type_info>()}
    {
    }


    std::optional<sym_tag_enum> symbol_type_info::sym_tag() const
    {
        if(cache_info_->has_sym_tag())
        {
            return cache_info_->sym_tag();
        }

        if(auto const rv = get_dword_type(TI_GET_SYMTAG, L"TI_GET_SYMTAG"sv, optional_type::optional); !rv.has_value())
        {
            cache_info_->set_sym_tag(std::nullopt);
        }
        else
        {
            cache_info_->set_sym_tag(static_cast<sym_tag_enum>(rv.value()));
        }

        return cache_info_->sym_tag();
    }

    std::optional<std::wstring_view> symbol_type_info::name() const
    {
        if(cache_info_->has_name())
        {
            return cache_info_->name();
        }

        wchar_t *name;
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, TI_GET_SYMNAME, &name))
        {
            cache_info_->set_name(std::nullopt);
            throw_sym_get_type_info_error(L"TI_GET_SYMNAME"sv, optional_type::optional);
            return std::nullopt;
        }

        cache_info_->set_name(name);
        LocalFree(name);

            return cache_info_->name();
    }

    std::optional<ULONG64> symbol_type_info::length() const
    {
        if(cache_info_->has_length())
        {
            return cache_info_->length();
        }

        if(auto const rv = get_unlong64_type(TI_GET_LENGTH, L"TI_GET_LENGTH"sv, optional_type::optional); !rv.has_value())
        {
            cache_info_->set_length(std::nullopt);
        }
        else
        {
            cache_info_->set_length(rv.value());
        }

        return cache_info_->length();
    }

    std::optional<symbol_type_info> symbol_type_info::type() const
    {
        if(cache_info_->has_type())
        {
            return cache_info_->type();
        }

        if(auto const type = get_dword_type(TI_GET_TYPE, L"TI_GET_TYPE"sv, optional_type::optional); !type.has_value())
        {
            cache_info_->set_type(std::nullopt);
        }
        else
        {
            cache_info_->set_type(symbol_type_info{module_base_, type.value()});
        }

        return cache_info_->type();
    }

    std::optional<symbol_type_info> symbol_type_info::type_id() const
    {
        auto const type = get_dword_type(TI_GET_TYPEID, L"TI_GET_TYPEID"sv, optional_type::optional);
        if(!type.has_value())
        {
            return std::nullopt;
        }

        return symbol_type_info{module_base_, type.value()};
    }

    std::optional<basic_type> symbol_type_info::base_type() const
    {
        auto const rv = get_dword_type(TI_GET_BASETYPE, L"TI_GET_BASETYPE"sv, optional_type::optional);
        if(!rv.has_value())
        {
            return std::nullopt;
        }
        return static_cast<basic_type>(rv.value());
    }

    std::optional<DWORD> symbol_type_info::array_index_typeid() const
    {
        return get_dword_type(TI_GET_ARRAYINDEXTYPEID, L"TI_GET_ARRAYINDEXTYPEID"sv, optional_type::optional);
    }

    std::optional<data_kind> symbol_type_info::data_kind() const
    {
        auto const rv = get_dword_type(TI_GET_DATAKIND, L"TI_GET_DATAKIND"sv, optional_type::optional);
        if(!rv.has_value())
        {
            return std::nullopt;
        }
        return static_cast<dbg_help::data_kind>(rv.value());
    }

    std::optional<DWORD> symbol_type_info::address_offset() const
    {
        return get_dword_type(TI_GET_ADDRESSOFFSET, L"TI_GET_ADDRESSOFFSET"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::offset() const
    {
        if(cache_info_->has_offset())
        {
            return cache_info_->offset();
        }

        if(auto const rv = get_dword_type(TI_GET_OFFSET, L"TI_GET_OFFSET"sv, optional_type::optional); !rv.has_value())
        {
            cache_info_->set_offset(std::nullopt);
        }
        else
        {
            cache_info_->set_offset(rv.value());
        }

        return cache_info_->offset();
    }

    std::optional<DWORD> symbol_type_info::array_count() const
    {
        return get_dword_type(TI_GET_COUNT, L"TI_GET_COUNT"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::children_count() const
    {
        return get_dword_type(TI_GET_CHILDRENCOUNT, L"TI_GET_CHILDRENCOUNT"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::bit_position() const
    {
        return get_dword_type(TI_GET_BITPOSITION, L"TI_GET_BITPOSITION"sv, optional_type::optional);
    }

    std::optional<bool> symbol_type_info::is_virtual_base_class() const
    {
        return get_bool_type(TI_GET_VIRTUALBASECLASS, L"TI_GET_VIRTUALBASECLASS"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::virtual_base_pointer_offset() const
    {
        return get_dword_type(TI_GET_VIRTUALBASEPOINTEROFFSET, L"TI_GET_VIRTUALBASEPOINTEROFFSET"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::class_parent_id() const
    {
        return get_dword_type(TI_GET_CLASSPARENTID, L"TI_GET_CLASSPARENTID"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::nested() const
    {
        return get_dword_type(TI_GET_NESTED, L"TI_GET_NESTED"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::sym_index_raw() const
    {
        return get_dword_type(TI_GET_SYMINDEX, L"TI_GET_SYMINDEX"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::lexical_parent() const
    {
        return get_dword_type(TI_GET_LEXICALPARENT, L"TI_GET_LEXICALPARENT"sv, optional_type::optional);
    }

    std::optional<ULONG64> symbol_type_info::address() const
    {
        return get_unlong64_type(TI_GET_ADDRESS, L"TI_GET_ADDRESS"sv, optional_type::optional);
    }

    std::optional<udt_kind_type> symbol_type_info::udt_kind() const
    {
        auto const rv = get_dword_type(TI_GET_UDTKIND, L"TI_GET_UDTKIND"sv, optional_type::optional);
        if(!rv.has_value())
        {
            return std::nullopt;
        }
        return static_cast<udt_kind_type>(rv.value());
    }

    std::optional<DWORD> symbol_type_info::virtual_base_offset() const
    {
        return get_dword_type(TI_GET_VIRTUALBASEOFFSET, L"TI_GET_VIRTUALBASEOFFSET"sv, optional_type::optional);
    }

    std::optional<DWORD> symbol_type_info::virtual_base_displacement_table_index() const
    {
        return get_dword_type(TI_GET_VIRTUALBASEDISPINDEX, L"TI_GET_VIRTUALBASEDISPINDEX"sv, optional_type::optional);
    }

    std::optional<bool> symbol_type_info::is_reference() const
    {
        return get_bool_type(TI_GET_IS_REFERENCE, L"TI_GET_IS_REFERENCE"sv, optional_type::optional);
    }

    std::optional<bool> symbol_type_info::indirect_virtual_base_class() const
    {
        return get_bool_type(TI_GET_INDIRECTVIRTUALBASECLASS, L"TI_GET_INDIRECTVIRTUALBASECLASS"sv, optional_type::optional);
    }

    std::optional<_variant_t> symbol_type_info::const_value() const
    {
        _variant_t rv;
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, TI_GET_VALUE, &rv))
        {
            throw_sym_get_type_info_error(L"TI_GET_VALUE"sv, optional_type::optional);
            return std::nullopt;
        }

        return std::move(rv);
    }

    std::optional<calling_convention> symbol_type_info::calling_convention() const
    {
        auto const rv = get_dword_type(TI_GET_CALLING_CONVENTION, L"TI_GET_CALLING_CONVENTION"sv, optional_type::optional);
        if(!rv.has_value())
        {
            return std::nullopt;
        }
        return static_cast<dbg_help::calling_convention>(rv.value());
    }

    std::optional<DWORD> symbol_type_info::get_dword_type(IMAGEHLP_SYMBOL_TYPE_INFO const type, std::wstring_view const& function, optional_type const optional) const
    {
        DWORD rv{0};
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, type, &rv))
        {
            throw_sym_get_type_info_error(function, optional);
            return std::nullopt;
        }

        return rv;
    }

    std::optional<ULONG64> symbol_type_info::get_unlong64_type(IMAGEHLP_SYMBOL_TYPE_INFO const type, std::wstring_view const& function, optional_type const optional) const
    {
        ULONG64 rv{0};
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, type, &rv))
        {
            throw_sym_get_type_info_error(function, optional);
            return std::nullopt;
        }

        return rv;
    }

    std::optional<bool> symbol_type_info::get_bool_type(IMAGEHLP_SYMBOL_TYPE_INFO const type, std::wstring_view const& function, optional_type const optional) const
    {
        BOOL rv{FALSE};
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, type, &rv))
        {
            throw_sym_get_type_info_error(function, optional);
            return std::nullopt;
        }

        return rv == TRUE;
    }

    void symbol_type_info::throw_sym_get_type_info_error(std::wstring_view const& function, optional_type const optional)
    {
        auto const ec = GetLastError();
        if(optional == optional_type::optional && (ec == ERROR_INVALID_FUNCTION || static_cast<HRESULT>(ec) == E_INVALIDARG))
        {
            return;
        }

        windows_error::throw_windows_api_error(L"SymGetTypeInfo"sv, function, ec);
    }

    std::experimental::generator<symbol_type_info> symbol_type_info::children() const  // NOLINT(bugprone-reserved-identifier)
    {
        if(!cache_info_->has_children())
        {
            auto const count = children_count();
            if(count.value_or(0) == 0) co_return;
            auto const size = sizeof(TI_FINDCHILDREN_PARAMS) + (sizeof(ULONG) * (count.value() - 1));
            auto const buffer = std::make_unique<char[]>(size);
            memset(buffer.get(), 0, size);
            auto* find_children_params = reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(buffer.get());
            find_children_params->Count = count.value();

            if(!SymGetTypeInfo(fake_process, module_base_, type_index_, TI_FINDCHILDREN, find_children_params))
            {
                throw_sym_get_type_info_error(L"TI_FINDCHILDREN"sv, optional_type::optional);
                co_return;
            }

            std::vector<symbol_type_info> cache_children;
            cache_children.reserve(count.value());
            for(ULONG index = 0; index < count.value(); ++index)
            {
                cache_children.emplace_back(module_base_, find_children_params->ChildId[index]);
            }

            cache_info_->set_children(std::move(cache_children));
        }

        for(auto const& child : cache_info_->children())
        {
            co_yield child;
        }
    }

    std::optional<std::pair<symbol_type_info, uint64_t>> symbol_type_info::find_field_in_type(std::wstring_view field_name) const
    {
        std::optional<std::pair<symbol_type_info, uint64_t>>  rv;
        if(cache_info_->find_field_in_type(field_name, rv))
        {
            return rv;
        }

        auto const lookup_field_name = field_name;
        symbol_type_info type = *this;
        uint64_t offset{0};
        while(true)
        {
            auto const pos = field_name.find_first_of(L'.');
            auto const find_name_part = pos == std::wstring_view::npos ? field_name : field_name.substr(0, pos);
            auto const find_name_rest = pos == std::wstring_view::npos ? std::wstring_view{} : field_name.substr(pos + 1);
            if(auto const type_tag_data = type.sym_tag(); type_tag_data.value_or(sym_tag_enum::Null) != sym_tag_enum::UDT)
            {
                cache_info_->set_find_field_in_type(lookup_field_name, std::nullopt);
                return rv;
            }

            auto found = false;
            for (auto const& child : type.children())
            {
                if(auto const name_data = child.name(); name_data.has_value() && string_compare::iequals(find_name_part, name_data.value()))
                {
                    if(find_name_rest.empty())
                    {
                        cache_info_->set_find_field_in_type(lookup_field_name, std::make_pair(child, offset));
                        rv = std::make_pair(child, offset);
                        return rv;
                    }

                    if(auto const child_type = child.type(); child_type.has_value())
                    {
                        if(auto const child_offset = child.offset(); child_offset.has_value())
                        {
                            found = true;
                            field_name = find_name_rest;
                            type = child_type.value();
                            offset += child_offset.value();
                            break;
                        }
                    }
                }
            }

            if(!found)
            {
                cache_info_->set_find_field_in_type(lookup_field_name, std::nullopt);
                return rv;
            }
        }
    }

    std::wstring symbol_type_info::to_address_string() const
    {
        return std::format(L"[0x{0:X}:0x{1:X}]", module_base_, type_index_);
    }

    std::optional<symbol_type_info> symbol_type_info::from_address_string(std::wstring_view const address)
    {
        if(address.empty())
        {
            return std::nullopt;
        }

        if(!address.starts_with(L'[') || !address.ends_with(L']'))
        {
            return std::nullopt;
        }

        auto const split_position = address.find_first_of(L':');
        if(split_position == std::wstring_view::npos)
        {
            return std::nullopt;
        }

        auto const module_base_string = address.substr(1, split_position - 2);
        auto const type_index_string = address.substr(split_position + 1, address.size() - (split_position + 2));

        if(module_base_string.empty() || type_index_string.empty())
        {
            return std::nullopt;
        }

        wchar_t* end_ptr{nullptr};
        auto const module_base = std::wcstoull(module_base_string.data(), &end_ptr, 0);
        if(end_ptr == nullptr || *end_ptr != L':')
        {
            return std::nullopt;
        }

        end_ptr = nullptr;
        auto const type_index = std::wcstoul(type_index_string.data(), &end_ptr, 0);
        if(end_ptr == nullptr || *end_ptr != L']')
        {
            return std::nullopt;
        }

        return symbol_type_info{module_base, type_index};
    }
}
