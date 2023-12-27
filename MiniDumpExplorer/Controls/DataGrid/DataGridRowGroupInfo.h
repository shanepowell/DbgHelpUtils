#pragma once

namespace DataGridInternal
{

class DataGridRowGroupInfo
{
public:
    DataGridRowGroupInfo() = default;
    DataGridRowGroupInfo(winrt::Microsoft::UI::Xaml::Data::ICollectionViewGroup const& collectionViewGroup,
                         winrt::Microsoft::UI::Xaml::Visibility visibility,
                         int32_t level,
                         int32_t slot,
                         int32_t lastSubItemSlot);

    winrt::Microsoft::UI::Xaml::Data::ICollectionViewGroup const& CollectionViewGroup() const { return collectionViewGroup_; }

    int32_t LastSubItemSlot() const { return lastSubItemSlot_; }
    void LastSubItemSlot(int32_t const value) { lastSubItemSlot_ = value; }

    int32_t Level() const { return level_; }

    int32_t Slot() const { return slot_; }
    void Slot(int32_t const value) { slot_ = value; }

    winrt::Microsoft::UI::Xaml::Visibility Visibility() const { return visibility_; }
    void Visibility(winrt::Microsoft::UI::Xaml::Visibility const value) { visibility_ = value; }

private:
    winrt::Microsoft::UI::Xaml::Data::ICollectionViewGroup collectionViewGroup_{nullptr};
    int32_t lastSubItemSlot_{};
    int32_t level_{};
    int32_t slot_{};
    winrt::Microsoft::UI::Xaml::Visibility visibility_{};
};

}