#pragma once
#include <generator>

namespace winrt::MiniDumpExplorer
{
    struct DataGrid;
    struct DataGridRow;
    struct DataGridRowGroupHeader;

    namespace implementation
    {
        struct DataGrid;
    }
}

namespace DataGridInternal
{

    class DataGridDisplayData
    {
    public:
        static constexpr auto InvalidIndex = int32_t{-1};

        DataGridDisplayData(winrt::MiniDumpExplorer::implementation::DataGrid* owner);

        int32_t FirstDisplayedScrollingCol() const { return firstDisplayedScrollingCol_; }
        void FirstDisplayedScrollingCol(int32_t const value) { firstDisplayedScrollingCol_ = value; }

        int32_t FirstScrollingSlot() const { return firstScrollingSlot_; }
        void FirstScrollingSlot(int32_t const value) { firstScrollingSlot_ = value; }

        int32_t LastScrollingSlot() const { return lastScrollingSlot_; }
        void LastScrollingSlot(int32_t const value) { lastScrollingSlot_ = value; }

        int32_t LastTotallyDisplayedScrollingCol() const { return lastTotallyDisplayedScrollingCol_; }
        void LastTotallyDisplayedScrollingCol(int32_t const value) { lastTotallyDisplayedScrollingCol_ = value; }

        size_t NumDisplayedScrollingElements() const { return scrollingElements_.size(); }

        int32_t NumTotallyDisplayedScrollingElements() const { return numTotallyDisplayedScrollingElements_; }
        void NumTotallyDisplayedScrollingElements(int32_t const value) { numTotallyDisplayedScrollingElements_ = value; }

        double PendingVerticalScrollHeight() const { return pendingVerticalScrollHeight_; }
        void PendingVerticalScrollHeight(double const value) { pendingVerticalScrollHeight_ = value; }

        void AddRecylableRow(winrt::MiniDumpExplorer::DataGridRow const& row);
        void AddRecylableRowGroupHeader(winrt::MiniDumpExplorer::DataGridRowGroupHeader const& groupHeader);
        void ClearElements(bool recycle);
        void CorrectSlotsAfterDeletion(int32_t slot, bool wasCollapsed);
        void CorrectSlotsAfterInsertion(int32_t slot, winrt::Microsoft::UI::Xaml::UIElement const& element, bool isCollapsed);
        void FullyRecycleElements();
        winrt::Microsoft::UI::Xaml::UIElement GetDisplayedElement(int32_t slot) const;
        winrt::MiniDumpExplorer::DataGridRow GetDisplayedRow(int32_t rowIndex) const;
        std::generator<winrt::Microsoft::UI::Xaml::UIElement> GetScrollingElements(bool onlyRows = false) const;
        winrt::MiniDumpExplorer::DataGridRowGroupHeader GetUsedGroupHeader();
        winrt::MiniDumpExplorer::DataGridRow GetUsedRow();
        void LoadScrollingSlot(int32_t slot, winrt::Microsoft::UI::Xaml::UIElement const& element, bool updateSlotInformation);
        void UnloadScrollingElement(int32_t slot, bool updateSlotInformation, bool wasDeleted);

    private:
        int32_t GetCircularListIndex(int32_t slot, bool wrap) const;
        void ResetSlotIndexes();
        void SetScrollingSlots(int32_t newValue);

    private:
        winrt::MiniDumpExplorer::implementation::DataGrid* owner_;
        int32_t firstDisplayedScrollingCol_{InvalidIndex};
        int32_t firstScrollingSlot_{0};
        int32_t lastScrollingSlot_{0};
        int32_t lastTotallyDisplayedScrollingCol_{InvalidIndex};
        int32_t numTotallyDisplayedScrollingElements_{0};
        double pendingVerticalScrollHeight_{0.0};
        int32_t headScrollingElements_{0};
        std::vector<winrt::Microsoft::UI::Xaml::UIElement> scrollingElements_;
        std::vector<winrt::MiniDumpExplorer::DataGridRow> recyclableRows_;
        std::vector<winrt::MiniDumpExplorer::DataGridRow> fullyRecycledRows_;
        std::vector<winrt::MiniDumpExplorer::DataGridRowGroupHeader> recyclableGroupHeaders_;
        std::vector<winrt::MiniDumpExplorer::DataGridRowGroupHeader> fullyRecycledGroupHeaders_;
    };

}
