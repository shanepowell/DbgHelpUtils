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

        GlobalOptionsNotifyPropertyChangedBase(std::vector<std::wstring_view> numberDisplayFormatProperties,
            std::vector<std::wstring_view> sizeNumberDisplayFormatProperties,
            std::vector<std::wstring_view> timeStampFormatProperties,
            std::vector<std::wstring_view> durationFormatProperties,
            std::vector<std::wstring_view> m128aFormatProperties,
            std::vector<std::wstring_view> floatingPointScientificDisplayFormatProperties)
            : numberDisplayFormatProperties_{std::move(numberDisplayFormatProperties)}
            , sizeNumberDisplayFormatProperties_{std::move(sizeNumberDisplayFormatProperties)}
            , timeStampFormatProperties_{std::move(timeStampFormatProperties)}
            , durationFormatProperties_{std::move(durationFormatProperties)}
            , m128aFormatProperties_{std::move(m128aFormatProperties)}
            , floatingPointScientificDisplayFormatProperties_{ std::move(floatingPointScientificDisplayFormatProperties) }
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
            if(!timeStampFormatProperties_.empty())
            {
                GlobalOptions::Options().OnTimeStampFormatChanged([this, ptr = Self()->get_weak()](auto const, auto const, auto const, auto const&, auto const, auto const&)
                    {
                        if(auto const self = ptr.get())
                        {
                            ProcessTimeStampFormatChanged();
                            return true;
                        }

                        return false;
                    });
            }
            if(!durationFormatProperties_.empty())
            {
                GlobalOptions::Options().OnDurationFormatChanged([this, ptr = Self()->get_weak()](auto const)
                    {
                        if(auto const self = ptr.get())
                        {
                            ProcessDurationFormatChanged();
                            return true;
                        }

                        return false;
                    });
            }
            if (!m128aFormatProperties_.empty())
            {
                GlobalOptions::Options().OnM128AViewDisplayFormatChanged([this, ptr = Self()->get_weak()](auto const)
                    {
                        if (auto const self = ptr.get())
                        {
                            ProcessM128AViewDisplayFormatChanged();
                            return true;
                        }
                        return false;
                    });
            }
            if (!floatingPointScientificDisplayFormatProperties_.empty())
            {
                GlobalOptions::Options().OnFloatingPointScientificDisplayFormatChanged([this, ptr = Self()->get_weak()](auto const)
                    {
                        if (auto const self = ptr.get())
                        {
                            ProcessFloatingPointScientificDisplayFormatChanged();
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

        virtual void OnTimeStampFormatChanged()
        {
        }

        virtual void OnDurationFormatChanged()
        {
        }

        virtual void OnM128AViewDisplayFormatChanged()
        {
        }

        virtual void OnFloatingPointScientificDisplayFormatChanged()
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

        void ProcessTimeStampFormatChanged()
        {
            RaisePropertiesChanged(timeStampFormatProperties_);
            OnTimeStampFormatChanged();
        }

        void ProcessDurationFormatChanged()
        {
            RaisePropertiesChanged(durationFormatProperties_);
            OnDurationFormatChanged();
        }

        void ProcessM128AViewDisplayFormatChanged()
        {
            RaisePropertiesChanged(m128aFormatProperties_);
            OnM128AViewDisplayFormatChanged();
        }

        void ProcessFloatingPointScientificDisplayFormatChanged()
        {
            RaisePropertiesChanged(floatingPointScientificDisplayFormatProperties_);
            OnFloatingPointScientificDisplayFormatChanged();
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
        std::vector<std::wstring_view> timeStampFormatProperties_;
        std::vector<std::wstring_view> durationFormatProperties_;
        std::vector<std::wstring_view> m128aFormatProperties_;
        std::vector<std::wstring_view> floatingPointScientificDisplayFormatProperties_;
    };
}