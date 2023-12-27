#include "pch.h"
#include "DataGridRowGroupInfo.h"

namespace DataGridInternal
{
    DataGridRowGroupInfo::DataGridRowGroupInfo(winrt::Microsoft::UI::Xaml::Data::ICollectionViewGroup const& collectionViewGroup
            , winrt::Microsoft::UI::Xaml::Visibility const visibility
            , int32_t const level
            , int32_t const slot
            , int32_t const lastSubItemSlot)
        : collectionViewGroup_{collectionViewGroup}
        , lastSubItemSlot_{lastSubItemSlot}
        , level_{level}
        , slot_{slot}
        , visibility_{visibility}
    {
    }
}
