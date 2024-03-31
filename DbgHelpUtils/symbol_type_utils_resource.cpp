#include "symbol_type_utils.h"

#include "flags_string_utils.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace dlg_help_utils::symbol_type_utils::resources
{
    using namespace dbg_help;

    namespace
    {
        std::unordered_map<sym_tag_enum, std::wstring> g_sym_tag_descriptions =
        {
            { sym_tag_enum::Null, L"(SymTagNull)"s },
            { sym_tag_enum::Exe, L"exe"s },
            { sym_tag_enum::Compiland, L"Compiland"s },
            { sym_tag_enum::CompilandDetails, L"CompilandDetails"s },
            { sym_tag_enum::CompilandEnv, L"CompilandEnv"s },
            { sym_tag_enum::Function, L"Function"s },
            { sym_tag_enum::Block, L"Block"s },
            { sym_tag_enum::Data, L"Data"s },
            { sym_tag_enum::Annotation, L"Annotation"s },
            { sym_tag_enum::Label, L"Label"s },
            { sym_tag_enum::PublicSymbol, L"PublicSymbol"s },
            { sym_tag_enum::UDT, L"UserDefinedType"s },
            { sym_tag_enum::Enum, L"ENUM"s },
            { sym_tag_enum::FunctionType, L"FunctionType"s },
            { sym_tag_enum::PointerType, L"PointerType"s },
            { sym_tag_enum::ArrayType, L"ArrayType"s },
            { sym_tag_enum::BaseType, L"BaseType"s },
            { sym_tag_enum::Typedef, L"Typedef"s },
            { sym_tag_enum::BaseClass, L"BaseClass"s },
            { sym_tag_enum::Friend, L"Friend"s },
            { sym_tag_enum::FunctionArgType, L"FunctionArgType"s },
            { sym_tag_enum::FuncDebugStart, L"FuncDebugStart"s },
            { sym_tag_enum::FuncDebugEnd, L"FuncDebugEnd"s },
            { sym_tag_enum::UsingNamespace, L"UsingNamespace"s },
            { sym_tag_enum::VTableShape, L"VTableShape"s },
            { sym_tag_enum::VTable, L"VTable"s },
            { sym_tag_enum::Custom, L"Custom"s },
            { sym_tag_enum::Thunk, L"Thunk"s },
            { sym_tag_enum::CustomType, L"CustomType"s },
            { sym_tag_enum::ManagedType, L"ManagedType"s },
            { sym_tag_enum::Dimension, L"Dimension"s },
            { sym_tag_enum::CallSite, L"CallSite"s },
            { sym_tag_enum::InlineSite, L"InlineSite"s },
            { sym_tag_enum::BaseInterface, L"BaseInterface"s },
            { sym_tag_enum::VectorType, L"VectorType"s },
            { sym_tag_enum::MatrixType, L"MatrixType"s },
            { sym_tag_enum::HLSLType, L"HLSLType"s },
            { sym_tag_enum::Caller, L"Caller"s },
            { sym_tag_enum::Callee, L"Callee"s },
            { sym_tag_enum::Export, L"Export"s },
            { sym_tag_enum::HeapAllocationSite, L"HeapAllocationSite"s },
            { sym_tag_enum::CoffGroup, L"CoffGroup"s },
            { sym_tag_enum::Inlinee, L"Inlinee"s },
        };

        std::unordered_map<basic_type, std::wstring> g_basic_type_descriptions =
        {
            { basic_type::NoType, L"<NoType>"s },
            { basic_type::Void, L"void"s },
            { basic_type::Char, L"char"s },
            { basic_type::WChar, L"wchar_t"s },
            { basic_type::Int, L"int"s },
            { basic_type::UInt, L"unsigned int"s },
            { basic_type::Float, L"float"s },
            { basic_type::BCD, L"<BCD>"s },
            { basic_type::Bool, L"bool"s },
            { basic_type::Long, L"long"s },
            { basic_type::ULong, L"unsigned long"s },
            { basic_type::Currency, L"<currency>"s },
            { basic_type::Date, L"<date>"s },
            { basic_type::Variant, L"VARIANT"s },
            { basic_type::Complex, L"<complex>"s },
            { basic_type::Bit, L"<bit>"s },
            { basic_type::BSTR, L"BSTR"s },
            { basic_type::HResult, L"HRESULT"s },
            { basic_type::Char16, L"char16_t"s },
            { basic_type::Char32, L"char32_t"s },
            { basic_type::Char8, L"char8_t"s },
        };

        std::unordered_map<udt_kind_type, std::wstring> g_udt_kind_type_descriptions =
        {
            { udt_kind_type::struct_type, L"struct"s },
            { udt_kind_type::class_type, L"class"s },
            { udt_kind_type::union_type, L"union"s },
            { udt_kind_type::interface_type, L"interface"s },
        };

        std::unordered_map<calling_convention, std::wstring> g_calling_convention_descriptions =
        {
            { calling_convention::near_c, L"near c"s },
            { calling_convention::far_c, L"far c"s },
            { calling_convention::near_pascal, L"near pascal"s },
            { calling_convention::far_pascal, L"far pascal"s },
            { calling_convention::near_fast, L"near fast"s },
            { calling_convention::far_fast, L"far fast"s },
            { calling_convention::skipped, L"skipped"s },
            { calling_convention::near_std, L"near std"s },
            { calling_convention::far_std, L"far std"s },
            { calling_convention::near_sys, L"near sys"s },
            { calling_convention::far_sys, L"far sys"s },
            { calling_convention::thiscall, L"thiscall"s },
            { calling_convention::mipscall, L"mipscall"s },
            { calling_convention::generic, L"generic"s },
            { calling_convention::alphacall, L"alphacall"s },
            { calling_convention::ppccall, L"ppccall"s },
            { calling_convention::shcall, L"shcall"s },
            { calling_convention::armcall, L"armcall"s },
            { calling_convention::am33call, L"am33call"s },
            { calling_convention::tricall, L"tricall"s },
            { calling_convention::sh5_call, L"sh5_call"s },
            { calling_convention::m32rcall, L"m32rcall"s },
            { calling_convention::clrcall, L"clrcall"s },
            { calling_convention::inlinecall, L"inline"s },
            { calling_convention::near_vector, L"near vector"s },
            { calling_convention::swift, L"swift"s },
            { calling_convention::reserved, L"reserved"s },
        };

        std::unordered_map<data_kind, std::wstring> g_data_kind_descriptions =
        {
            { data_kind::data_is_local, L"local"s },
            { data_kind::data_is_unknown, L"unknown"s },
            { data_kind::data_is_static_local, L"static local"s },
            { data_kind::data_is_param, L"param"s },
            { data_kind::data_is_object_ptr, L"object ptr"s },
            { data_kind::data_is_file_static, L"file static"s },
            { data_kind::data_is_global, L"global"s },
            { data_kind::data_is_member, L"member"s },
            { data_kind::data_is_static_member, L"static member"s },
            { data_kind::data_is_constant, L"constant"s },
        };
    }

    std::wstring sym_tag_to_string(sym_tag_enum const type)
    {
        return flags_string_utils::generate_enum_string(type, g_sym_tag_descriptions);
    }

    std::wstring basic_type_to_string(basic_type const type)
    {
        return flags_string_utils::generate_enum_string(type, g_basic_type_descriptions);
    }

    std::wstring udt_kind_to_string(udt_kind_type const type)
    {
        return flags_string_utils::generate_enum_string(type, g_udt_kind_type_descriptions);
    }

    std::wstring calling_convention_to_string(calling_convention const type)
    {
        return flags_string_utils::generate_enum_string(type, g_calling_convention_descriptions);
    }

    std::wstring data_kind_convention_to_string(data_kind const type)
    {
        return flags_string_utils::generate_enum_string(type, g_data_kind_descriptions);
    }

    std::wstring_view get_symbol_name_none()
    {
        return L"(none)"sv;
    }

    std::wstring get_unsupported_type()
    {
        return L"unsupported type"s;
    }

    std::wstring get_on_type()
    {
        return L"on type"s;
    }

    std::wstring get_variable_unknown()
    {
        return L"<unknown>"s;
    }

    std::wstring get_bcd_value_unsupported()
    {
        return L"BCD value unsupported"s;
    }

    std::wstring get_currency_value_unsupported()
    {
        return L"Currency value unsupported"s;
    }

    std::wstring get_date_value_unsupported()
    {
        return L"Date value unsupported"s;
    }

    std::wstring get_variant_value_unsupported()
    {
        return L"Variant value unsupported"s;
    }

    std::wstring get_complex_value_unsupported()
    {
        return L"Complex value unsupported"s;
    }

    std::wstring get_bstr_value_unsupported()
    {
        return L"BSTR value unsupported"s;
    }

    std::wstring get_bits_title()
    {
        return L"bits"s;
    }

    std::wstring get_bit_title()
    {
        return L"bit"s;
    }

    std::wstring get_enum_title()
    {
        return L"enum"s;
    }

    std::wstring get_reference_title()
    {
        return L"&"s;
    }

    std::wstring get_pointer_title()
    {
        return L"*"s;
    }

    std::wstring get_int8_title()
    {
        return L"int8_t"s;
    }

    std::wstring get_int16_title()
    {
        return L"int16_t"s;
    }

    std::wstring get_int32_title()
    {
        return L"int32_t"s;
    }

    std::wstring get_int64_title()
    {
        return L"int64_t"s;
    }

    std::wstring get_int128_title()
    {
        return L"int128_t"s;
    }

    std::wstring get_int256_title()
    {
        return L"int256_t"s;
    }

    std::wstring get_uint8_title()
    {
        return L"uint8_t"s;
    }

    std::wstring get_uint16_title()
    {
        return L"uint16_t"s;
    }

    std::wstring get_uint32_title()
    {
        return L"uint32_t"s;
    }

    std::wstring get_uint64_title()
    {
        return L"uint64_t"s;
    }

    std::wstring get_uint128_title()
    {
        return L"uint128_t"s;
    }

    std::wstring get_uint256_title()
    {
        return L"uint256_t"s;
    }

    std::wstring get_float32_title()
    {
        return L"float32"s;
    }

    std::wstring get_float64_title()
    {
        return L"float64"s;
    }

    std::wstring get_float128_title()
    {
        return L"float128"s;
    }

    std::wstring get_float256_title()
    {
        return L"float256"s;
    }

    std::wstring get_failed_to_find_string()
    {
        return L"failed to find"s;
    }

    std::wstring get_symbol_title()
    {
        return L"symbol"s;
    }

    std::wstring get_is_zero_length_string()
    {
        return L"is zero length"s;
    }

    std::wstring get_memory_address_in_range_string()
    {
        return L"memory address range in dump file"s;
    }
}
