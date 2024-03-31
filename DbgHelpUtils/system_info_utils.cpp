// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers
#include "system_info_utils.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include <avrfsdk.h>
#include <map>
#include <unordered_map>

// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

#include "flags_string_utils.h"
#include "mini_dump.h"
#include "stream_hex_dump.h"
#include "system_info_utils_resource.h"


// #define MEM_EXECUTE_OPTION_VALID_FLAGS 0x3f

namespace dlg_help_utils::system_info_utils::resources
{
    enum class platform_type : uint16_t;
}

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::system_info_utils::resources;



namespace
{
}


namespace dlg_help_utils::system_info_utils
{
    namespace resources
    {
        std::wstring to_wstring(windows_version const& version)
        {
            return std::format(L"{}.{} {}", version.major_version(), version.minor_version(), to_wstring(version.type()));
        }

        std::wstring to_wstring(file_type const& file)
        {
            return std::format(L"{}:{}", stream_hex_dump::to_hex(file.type()), stream_hex_dump::to_hex(file.sub_type()));
        }

        std::filesystem::path init_running_application_path()
        {
            if (wchar_t tmp_buffer[MAX_PATH]; ::GetModuleFileName(nullptr, tmp_buffer, MAX_PATH) > 0)
            {
                return {tmp_buffer};
            }

            return {};
        }

        std::filesystem::path init_temp_directory()
        {
            if (wchar_t tmp_buffer[MAX_PATH + 2]; GetTempPathW(MAX_PATH + 2, tmp_buffer) > 0)
            {
                return {tmp_buffer};
            }

            return {};
        }
    }

    std::wstring version_info_to_string(uint32_t const version)
    {
        return std::format(L"{0}.{1}", HIWORD(version), LOWORD(version));
    }

    std::wstring version_info_to_string(uint32_t const version_ms, uint32_t const version_ls)
    {
        return std::format(L"{0}.{1}.{2}.{3}", HIWORD(version_ms), LOWORD(version_ms), HIWORD(version_ls), LOWORD(version_ls));
    }

    std::filesystem::path const& get_running_application_path()
    {
        static auto running_application_path{init_running_application_path()};
        return running_application_path;
    }

    std::filesystem::path const& get_run_from_directory()
    {
        static auto run_from_directory{get_running_application_path().parent_path()};
        return run_from_directory;
    }

    std::filesystem::path const& get_temp_directory()
    {
        static auto temp_directory{init_temp_directory()};
        return temp_directory;
    }
}
