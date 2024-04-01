#pragma once
#include "NotifyPropertyChangedBase.h"
#include "Helpers/GlobalOptions.h"

namespace winrt::MiniDumpExplorer::implementation
{
    template<typename T>
    struct GlobalOptionsNotifyPropertyChangedBase : NotifyPropertyChangedBase<T>
    {
    protected:
        using NotifyPropertyChangedBase<T>::Self;
        using NotifyPropertyChangedBase<T>::RaisePropertyChanged;

        GlobalOptionsNotifyPropertyChangedBase(std::vector<std::wstring_view> numberDisplayFormatProperties, std::vector<std::wstring_view> sizeNumberDisplayFormatProperties)
            : numberDisplayFormatProperties_{std::move(numberDisplayFormatProperties)}
            , sizeNumberDisplayFormatProperties_{std::move(sizeNumberDisplayFormatProperties)}
        {
            if(!numberDisplayFormatProperties_.empty())
            {
                GlobalOptions::Options().OnNumberDisplayFormatChanged([this, ptr = Self()->get_weak()](auto const)
                    {
                        if(auto const self = ptr.get())
                        {
                            ProcessNumberDisplayFormatChanged();
                            return true;
                        }

                        return false;
                    });
            }
            if(!sizeNumberDisplayFormatProperties_.empty())
            {
                GlobalOptions::Options().OnSizeNumberDisplayFormatChanged([this, ptr = Self()->get_weak()](auto const, auto const, const auto)
                    {
                        if(auto const self = ptr.get())
                        {
                            ProcessSizeNumberDisplayFormatChanged();
                            return true;
                        }

                        return false;
                    });
            }
        }

        virtual void OnNumberDisplayFormatChanged()
        {
        }

        virtual void OnSizeNumberDisplayFormatChanged()
        {
        }

    private:
        void ProcessNumberDisplayFormatChanged()
        {
            RaisePropertiesChanged(numberDisplayFormatProperties_);
            OnNumberDisplayFormatChanged();
        }

        void ProcessSizeNumberDisplayFormatChanged()
        {
            RaisePropertiesChanged(sizeNumberDisplayFormatProperties_);
            OnSizeNumberDisplayFormatChanged();
        }

        void RaisePropertiesChanged(std::vector<std::wstring_view> const& properties)
        {
            for(auto const& propertyName : properties)
            {
                RaisePropertyChanged(propertyName);
            }
        }


    private:
        std::vector<std::wstring_view> numberDisplayFormatProperties_;
        std::vector<std::wstring_view> sizeNumberDisplayFormatProperties_;
    };
}