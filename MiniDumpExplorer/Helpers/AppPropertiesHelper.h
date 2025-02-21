#pragma once

class AppPropertiesHelper
{
public:
    template<typename T>
    static void SetEnumProperty(std::wstring const& propertyName, T const value)
    {
        SetIntProperty(propertyName, static_cast<int>(value));
    }

    template<typename T>
    static T GetEnumProperty(std::wstring const& propertyName, T const defaultValue)
    {
        return static_cast<T>(GetIntProperty(propertyName, static_cast<int>(defaultValue)));
    }

    static void SetStringProperty(std::wstring const& propertyName, std::wstring const& value);
    static std::wstring GetStringProperty(std::wstring const& propertyName, std::wstring const& defaultValue = {});
    static void SetIntProperty(std::wstring const& propertyName, int value);
    static int GetIntProperty(std::wstring const& propertyName, int defaultValue);
    static void SetUnsignedIntProperty(std::wstring const& propertyName, unsigned int value);
    static unsigned int GetUnsignedIntProperty(std::wstring const& propertyName, unsigned int defaultValue);
    static void SetStringVectorProperty(std::wstring const& propertyName, std::vector<std::wstring> const& value);
    static std::vector<std::wstring> GetStringVectorProperty(std::wstring const& propertyName, std::vector<std::wstring> const& defaultValue = {});
    static void SetBoolProperty(std::wstring const& propertyName, bool value);
    static bool GetBoolProperty(std::wstring const& propertyName, bool defaultValue);

    static void Initialize(std::wstring const& appName);

private:
    static void ValidateAppName();

private:
    static std::wstring _appRegistryPath;
};
