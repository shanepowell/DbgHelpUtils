#include "symbol_type_info.h"

#include "symbol_engine.h"
#include "windows_error.h"

#include <format>

using namespace std::string_view_literals;

namespace dlg_help_utils::dbg_help
{
    symbol_type_info::symbol_type_info(DWORD64 const module_base, ULONG const type_index)
    : module_base_{module_base}
    , type_index_{type_index}
    {
    }


    std::optional<enum sym_tag_enum> symbol_type_info::sym_tag() const
    {
        auto const rv = get_dword_type(TI_GET_SYMTAG, L"TI_GET_SYMTAG"sv, optional_type::optional);
        if(!rv.has_value())
        {
            return std::nullopt;
        }
        return static_cast<enum sym_tag_enum>(rv.value());
    }

    std::optional<std::wstring> symbol_type_info::name() const
    {
        wchar_t *name;
        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, TI_GET_SYMNAME, &name))
        {
            throw_sym_get_type_info_error(L"TI_GET_SYMNAME"sv, optional_type::optional);
            return std::nullopt;
        }
        std::wstring rv{name};
        LocalFree(name);
        return std::move(rv);
    }

    std::optional<ULONG64> symbol_type_info::length() const
    {
        return get_unlong64_type(TI_GET_LENGTH, L"TI_GET_LENGTH"sv, optional_type::optional);
    }

    std::optional<symbol_type_info> symbol_type_info::type() const
    {
        auto const type = get_dword_type(TI_GET_TYPE, L"TI_GET_TYPE"sv, optional_type::optional);
        if(!type.has_value())
        {
            return std::nullopt;
        }

        return symbol_type_info{module_base_, type.value()};
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
        return get_dword_type(TI_GET_OFFSET, L"TI_GET_OFFSET"sv, optional_type::optional);
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
        auto const count = children_count();
        if(count.value_or(0) == 0) co_return;
        auto const size = sizeof(TI_FINDCHILDREN_PARAMS) + (sizeof(ULONG) * (count.value() - 1));
        auto const buffer = std::make_unique<char[]>(size);
        memset(buffer.get(), 0, size);
        auto* find_children_params = reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(buffer.get());
        find_children_params->Count = count.value();

        if(!SymGetTypeInfo(fake_process, module_base_, type_index_, TI_FINDCHILDREN, find_children_params))
        {
            throw_sym_get_type_info_error(L"TI_FINDCHILDREN"sv, optional_type::required);
        }

        for(ULONG index = 0; index < count.value(); ++index)
        {
            co_yield symbol_type_info{module_base_, find_children_params->ChildId[index]};
        }
    }
}
