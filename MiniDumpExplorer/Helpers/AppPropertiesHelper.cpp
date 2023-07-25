#include "pch.h"
#include "AppPropertiesHelper.h"

#include <numeric>

#include "DbgHelpUtils/handles.h"

namespace
{
    auto open_registry_key(HKEY const root, std::wstring const& subKey, REGSAM const access = KEY_ALL_ACCESS)
    {
        HKEY key{};
        if (auto const result = RegCreateKeyExW(root, subKey.c_str(), 0, nullptr, 0, access, nullptr, &key, nullptr); result != ERROR_SUCCESS)
        {
            throw std::runtime_error("Failed to open registry key: " + std::to_string(result));
        }

        return dlg_help_utils::handles::make_registry_key_handle(key);
    }

    void set_registry_value(HKEY const key, std::wstring const& valueName, DWORD const type, void const* data, size_t const dataSize)
    {
        if (auto const result = RegSetValueExW(key, valueName.c_str(), 0, type, static_cast<BYTE const*>(data), static_cast<DWORD>(dataSize)); result != ERROR_SUCCESS)
        {
            throw std::runtime_error("Failed to set registry value: " + std::to_string(result));
        }
    }

    std::optional<DWORD> get_registry_value(HKEY const key, std::wstring const& valueName, DWORD const type, void* data, DWORD dataSize)
    {
        DWORD actualType{};
        if (auto const result = RegQueryValueExW(key, valueName.c_str(), nullptr, &actualType, static_cast<BYTE*>(data), reinterpret_cast<LPDWORD>(&dataSize)); result != ERROR_SUCCESS)
        {
            if(result == ERROR_MORE_DATA)
            {
                return dataSize;
            }
            if(result == ERROR_FILE_NOT_FOUND)
            {
                return std::nullopt;
            }

            throw std::runtime_error("Failed to get registry value: " + std::to_string(result));
        }

        if (actualType != type)
        {
            throw std::runtime_error("Registry value type mismatch");
        }

        return dataSize;
    }
}

std::wstring AppPropertiesHelper::_appRegistryPath{};

void AppPropertiesHelper::SetStringProperty(std::wstring const& propertyName, std::wstring const& value)
{
    ValidateAppName();
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);
    set_registry_value(key.get(), propertyName, REG_SZ, value.c_str(), (value.size() + 1) * sizeof(wchar_t));
}

std::wstring AppPropertiesHelper::GetStringProperty(std::wstring const& propertyName, std::wstring const& defaultValue)
{
    ValidateAppName();
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);
    std::wstring value;
    value.resize(256);
    auto size = get_registry_value(key.get(), propertyName, REG_SZ, value.data(), static_cast<DWORD>(sizeof(wchar_t) * (value.length() - 1)));
    if(!size.has_value())
    {
        return defaultValue;
    }
    while(size.value() > sizeof(wchar_t) * (value.length() - 1))
    {
        value.resize((size.value() + 1) / sizeof(wchar_t));
        size = get_registry_value(key.get(), propertyName, REG_SZ, value.data(), static_cast<DWORD>(sizeof(wchar_t) * (value.length() - 1)));

        if(!size.has_value())
        {
            return defaultValue;
        }
    }

    // trim null characters off the end of the string
    auto length = size.value() / sizeof(wchar_t);
    while(length > 0 && value[length - 1] == L'\0')
    {
        --length;
    }

    value.resize(length);
    return value;
}

void AppPropertiesHelper::SetIntProperty(std::wstring const& propertyName, int const value)
{
    ValidateAppName();
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);
    set_registry_value(key.get(), propertyName, REG_DWORD, &value, sizeof(value));
}

int AppPropertiesHelper::GetIntProperty(std::wstring const& propertyName, int const defaultValue)
{
    ValidateAppName();
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);
    int value{};
    if(auto const size = get_registry_value(key.get(), propertyName, REG_DWORD, &value, sizeof(value));
        !size.has_value() || size.value() != sizeof(value))
    {
        return defaultValue;
    }

    return value;
}

void AppPropertiesHelper::SetStringVectorProperty(std::wstring const& propertyName, std::vector<std::wstring> const& value)
{
    ValidateAppName();
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);

    std::wstring multiSz;
    multiSz.reserve(std::accumulate(value.begin(), value.end(), static_cast<std::wstring::size_type>(0), [](std::wstring::size_type const length, std::wstring const& v) { return length + v.size() + 1; }));
    for (auto const& v : value)
    {
        multiSz.append(v);
        multiSz.push_back(L'\0');
    }

    set_registry_value(key.get(), propertyName, REG_MULTI_SZ, multiSz.c_str(), (value.size() + 1) * sizeof(wchar_t));
}

std::vector<std::wstring> AppPropertiesHelper::GetStringVectorProperty(std::wstring const& propertyName, std::vector<std::wstring> const& defaultValue)
{
    auto const key = open_registry_key(HKEY_CURRENT_USER, _appRegistryPath);
    std::wstring value;
    value.resize(256);
    auto size = get_registry_value(key.get(), propertyName, REG_MULTI_SZ, value.data(), static_cast<DWORD>(sizeof(wchar_t) * (value.length() - 1)));
    if(!size.has_value())
    {
        return defaultValue;
    }
    while(size.value() > sizeof(wchar_t) * (value.length() - 1))
    {
        value.resize((size.value() + 1) / sizeof(wchar_t));
        size = get_registry_value(key.get(), propertyName, REG_SZ, value.data(), static_cast<DWORD>(sizeof(wchar_t) * (value.length() - 1)));

        if(!size.has_value())
        {
            return defaultValue;
        }
    }

    std::vector<std::wstring> result;
    auto previousPos = std::wstring::npos;
    auto pos = value.find(L'\0');
    while(pos != std::wstring::npos)
    {
        if(previousPos != std::wstring::npos && pos == previousPos + 1)
        {
            // double null indicates end of multi-sz
            break;
        }
        result.emplace_back(value.substr(previousPos + 1, pos - previousPos - 1));
        previousPos = pos;
        pos = value.find(L'\0', pos + 1);
    }

    if(pos == std::wstring::npos && previousPos != std::wstring::npos && previousPos + 1 < value.size())
    {
        // is there a left over string? - i.e. no double null
        result.emplace_back(value.substr(previousPos + 1));
    }

    return result;
}

void AppPropertiesHelper::Initialize(std::wstring const& appName)
{
    _appRegistryPath = std::format(L"Software\\{}", appName);
}

void AppPropertiesHelper::ValidateAppName()
{
    if (_appRegistryPath.empty())
    {
        throw std::runtime_error("AppPropertiesHelper::Initialize must be called before using AppPropertiesHelper");
    }
}
