#include "file_version_info.h"

#include <format>

#include "mini_dump_memory_stream.h"
#include "pe_file.h"

#pragma comment(lib, "Version.lib")

using namespace std::string_view_literals;

namespace dlg_help_utils
{
    file_version_info::file_version_info(std::wstring const& filename)
    {
        if(filename.empty())
        {
            return;
        }

        // allocate a block of memory for the version info
        DWORD dummy;
        const auto dw_size = GetFileVersionInfoSizeW(filename.c_str(), &dummy);
        if (dw_size == 0)
        {
            return;
        }

        file_version_info_.resize(dw_size);

        // load the version info
        if (!GetFileVersionInfoW(filename.c_str(), NULL, dw_size, file_version_info_.data()))
        {
            file_version_info_.clear();
            return;
        }
    }

    file_version_info::file_version_info(mini_dump_memory_stream stream)
    {
        pe_file pe{std::move(stream)};
        pe.open_pe();
        if(!pe.is_valid())
        {
            return;
        }

        file_version_info_ = pe.version_resource_data();
    }

    std::wstring file_version_info::product_name(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"ProductName"sv, language, codepage);
    }

    std::wstring file_version_info::product_version(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"ProductVersion"sv, language, codepage);
    }

    std::wstring file_version_info::file_version(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"FileVersion"sv, language, codepage);
    }

    std::wstring file_version_info::file_description(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"FileDescription"sv, language, codepage);
    }

    std::wstring file_version_info::company_name(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"CompanyName"sv, language, codepage);
    }

    std::wstring file_version_info::legal_copyright(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"LegalCopyright"sv, language, codepage);
    }

    std::wstring file_version_info::legal_trademarks(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"LegalTrademarks"sv, language, codepage);
    }

    std::wstring file_version_info::private_build(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"PrivateBuild"sv, language, codepage);
    }

    std::wstring file_version_info::special_build(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"SpecialBuild"sv, language, codepage);
    }

    std::wstring file_version_info::original_filename(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"OriginalFilename"sv, language, codepage);
    }

    std::wstring file_version_info::comments(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"Comments"sv, language, codepage);
    }

    std::wstring file_version_info::internal_name(uint32_t const language, uint32_t const codepage) const
    {
        return load_resource_string(L"InternalName"sv, language, codepage);
    }

    std::optional<VS_FIXEDFILEINFO> file_version_info::file_info() const
    {
        VS_FIXEDFILEINFO* fixedFileInfo = nullptr;

        unsigned int length = 0;
        VerQueryValueW(file_version_info_.data(), 
                      L"\\",
                      reinterpret_cast<LPVOID*>(&fixedFileInfo),
                      &length);

        if(fixedFileInfo == nullptr)
        {
            return std::nullopt;
        }

        return *fixedFileInfo;
    }

    std::wstring file_version_info::load_resource_string(std::wstring_view const resource, uint32_t language, uint32_t codepage) const
    {
        if(language == default_value)
        {
            language = GetUserDefaultUILanguage();
        }

        if(codepage == default_value)
        {
            codepage = GetACP();
        }

        struct LangAndCodepage {
          WORD Language;
          WORD CodePage;
        } *translations;

        unsigned int i_translate_len = 0;

        // Read the list of languages and code pages.
        VerQueryValueW(file_version_info_.data(), 
                      L"\\VarFileInfo\\Translation",
                      reinterpret_cast<LPVOID*>(&translations),
                      &i_translate_len);

        auto const translations_count = i_translate_len/sizeof(LangAndCodepage);
        auto const* translations_begin = translations;
        auto const* translations_end = translations_begin + translations_count;

        if(translations_count <= 0)
        {
            return {};
        }

        auto it = std::find_if(translations_begin, translations_end, [language, codepage](LangAndCodepage const& transaction) { return transaction.Language == language && transaction.CodePage == codepage; });
        if(it != translations_end)
        {
            return get_string_resource_string_for(resource, it->Language, it->CodePage);
        }

        it = std::find_if(translations_begin, translations_end, [language](LangAndCodepage const& transaction) { return transaction.Language == language; });
        if(it != translations_end)
        {
            return get_string_resource_string_for(resource, it->Language, it->CodePage);
        }

        it = std::find_if(translations_begin, translations_end, [codepage](LangAndCodepage const& transaction) { return transaction.CodePage == codepage; });
        if(it != translations_end)
        {
            return get_string_resource_string_for(resource, it->Language, it->CodePage);
        }

        // give up and return the first language/codepage string
        return get_string_resource_string_for(resource, translations_begin->Language, translations_begin->CodePage);
    }

    std::wstring file_version_info::get_string_resource_string_for(std::wstring_view const resource, uint32_t const language, uint32_t const codepage) const
    {
        LPVOID pv_resource_string = nullptr;
        unsigned int i_resource_len = 0;

        auto const subBlock = std::format(L"\\StringFileInfo\\{0:0>4X}{1:0>4X}\\{2}", language, codepage, resource);
        if (!VerQueryValueW(file_version_info_.data(), subBlock.c_str(), &pv_resource_string, &i_resource_len))
        {
            return {};
        }

        return {static_cast<LPCWSTR>(pv_resource_string)};
    }
}
