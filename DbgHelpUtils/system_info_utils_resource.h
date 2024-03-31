#pragma once
#include <cstdint>

// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include "hash_combine.h"

namespace dlg_help_utils::system_info_utils::resources
{
    enum class mem_execute_options : uint32_t
    {
        MEM_EXECUTE_OPTION_DISABLE = 0x1,
        MEM_EXECUTE_OPTION_ENABLE = 0x2,
        MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION = 0x4,
        MEM_EXECUTE_OPTION_PERMANENT = 0x8,
        MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE = 0x10,
        MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE = 0x20
    };

    enum class platform_type : uint16_t
    {
        workstation,
        server,
        server_2019,
        server_v_next,
        home_server,
        professional_edition
    };

    class windows_version
    {
    public:
        constexpr windows_version(uint32_t const major_version, uint32_t const minor_version,
            uint32_t const build_number, unsigned char const product_type,
            unsigned short const processor_architecture, unsigned short const suite_mask)
            : type_{ platform_type::workstation }
            , major_version_{ major_version }
            , minor_version_{ minor_version }
        {
            if (major_version_ == 10 && minor_version == 0)
            {
                if (product_type == VER_NT_WORKSTATION)
                {
                    type_ = platform_type::workstation;
                }
                else if (build_number > 17763)
                {
                    type_ = platform_type::server_v_next;
                }
                else if (build_number > 17622)
                {
                    type_ = platform_type::server_2019;
                }
                else
                {
                    type_ = platform_type::server;
                }
            }
            else if (major_version_ >= 6)
            {
                type_ = product_type == VER_NT_WORKSTATION ? platform_type::workstation : platform_type::server;
            }
            else if (major_version == 5)
            {
                if (minor_version == 2)
                {
                    if (suite_mask & VER_SUITE_WH_SERVER)
                    {
                        type_ = platform_type::home_server;
                    }
                    else if (product_type == VER_NT_WORKSTATION)
                    {
                        type_ = processor_architecture == PROCESSOR_ARCHITECTURE_AMD64
                            ? platform_type::professional_edition
                            : platform_type::server;
                    }
                    else
                    {
                        type_ = platform_type::server;
                    }
                }
                else
                {
                    type_ = product_type == VER_NT_WORKSTATION ? platform_type::workstation : platform_type::server;
                }
            }
            else
            {
                type_ = platform_type::workstation;
            }
        }

        auto operator<=>(windows_version const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)

        [[nodiscard]] std::size_t hash() const
        {
            std::size_t hash{ 0 };
            hash_combine(hash, std::hash<platform_type>{}(type_));
            hash_combine(hash, std::hash<uint32_t>{}(major_version_));
            hash_combine(hash, std::hash<uint32_t>{}(minor_version_));
            return hash;
        }

        [[nodiscard]] platform_type type() const { return type_; }
        [[nodiscard]] uint32_t major_version() const { return major_version_; }
        [[nodiscard]] uint32_t minor_version() const { return minor_version_; }


    private:
        platform_type type_;
        uint32_t major_version_;
        uint32_t minor_version_;
    };

    std::wstring to_wstring(platform_type type);
    std::wstring to_wstring(windows_version const& version);
    std::filesystem::path init_running_application_path();
    std::filesystem::path init_temp_directory();

    namespace windows_versions
    {
        // see https://en.wikipedia.org/wiki/Windows_Server_2022
        constexpr windows_version windows_server_2022{10, 0, 20348, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2022_preview{10, 0, 20285, VER_NT_SERVER, 0, 0};

        // see https://en.wikipedia.org/wiki/Windows_Server_2019
        constexpr windows_version windows_server_2019_2004{10, 0, 19042, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019_2009_20H2{10, 0, 18342, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019_1903{10, 0, 19041, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019_1909{10, 0, 17763, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019_20H2{10, 0, 17623, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_vnext{10, 0, 17764, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019{10, 0, 19042, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2019_datacenter{10, 0, 17134, VER_NT_SERVER, 0, 0};

        constexpr windows_version windows_server_2016_fall_creators_update{10, 0, 16299, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2016{10, 0, 0, VER_NT_SERVER, 0, 0};

        constexpr windows_version windows_server_2012_R2{6, 3, 0, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2012{6, 2, 0, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2008_R2{6, 1, 0, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_2008{6, 0, 0, VER_NT_SERVER, 0, 0};
        constexpr windows_version windows_server_home_server{5, 2, 0, 0, 0, VER_SUITE_WH_SERVER};
        constexpr windows_version windows_server_2003{5, 2, 0, 0, 0, 0};

        // see https://en.wikipedia.org/wiki/Windows_11_version_history
        constexpr windows_version windows_11_23H2{10, 0, 22631, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_11_22H2{10, 0, 22621, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_11{10, 0, 22000, VER_NT_WORKSTATION, 0, 0};

        // see https://en.wikipedia.org/wiki/Windows_10_version_history
        constexpr windows_version windows_10_22H2{10, 0, 19045, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_21H2{10, 0, 19044, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_21H1{10, 0, 19043, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_20H2{10, 0, 19042, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_2004{10, 0, 19041, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_1909{10, 0, 18363, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_1903{10, 0, 18362, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_october_2018{10, 0, 17763, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_april_2018{10, 0, 17134, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_fall_creators_update{10, 0, 16299, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_creators_update{10, 0, 15063, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_anniversary_update{10, 0, 14393, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_november_update{10, 0, 10586, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10_gold{10, 0, 10240, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_10{10, 0, 0, VER_NT_WORKSTATION, 0, 0};

        constexpr windows_version windows_8_1{6, 3, 0, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_8{6, 2, 0, VER_NT_WORKSTATION, 0, 0};

        constexpr windows_version windows_7{6, 1, 0, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_vista{6, 0, 0, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_server{5, 2, 0, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_xp{5, 1, 0, VER_NT_WORKSTATION, 0, 0};
        constexpr windows_version windows_2000{5, 0, 0, VER_NT_WORKSTATION, 0, 0};
    }


    class file_type
    {
    public:
        file_type(uint32_t const file_type, uint32_t const file_sub_type = std::numeric_limits<uint32_t>::max())
        : file_type_(file_type)
        , file_sub_type_(file_sub_type)
        {
        }

        auto operator<=>(file_type const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)
        auto operator==(file_type const& b) const
        {
            return file_type_ == b.file_type_ && (file_sub_type_ == std::numeric_limits<uint32_t>::max() || b.file_sub_type_ == std::numeric_limits<uint32_t>::max() || file_sub_type_ == b.file_sub_type_);
        }

        [[nodiscard]] std::size_t hash() const
        {
            std::size_t hash{ 0 };
            hash_combine(hash, std::hash<uint32_t>{}(file_type_));
            hash_combine(hash, std::hash<uint32_t>{}(file_sub_type_));
            return hash;
        }

        [[nodiscard]] uint32_t type() const { return file_type_; }
        [[nodiscard]] uint32_t sub_type() const { return file_sub_type_; }

    private:
        uint32_t file_type_;
        uint32_t file_sub_type_;
    };

    std::wstring to_wstring(file_type const& version);
}


template <>
struct std::hash<dlg_help_utils::system_info_utils::resources::windows_version>
{
    std::size_t operator()(dlg_help_utils::system_info_utils::resources::windows_version const& v) const noexcept
    {
        return v.hash();
    }
};

template <>
struct std::hash<dlg_help_utils::system_info_utils::resources::file_type>
{
    std::size_t operator()(dlg_help_utils::system_info_utils::resources::file_type const& v) const noexcept
    {
        return v.hash();
    }
};
