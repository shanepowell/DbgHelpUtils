#pragma once

#include <experimental/generator>
#include <vector>


namespace winrt::MiniDumpExplorer
{
    struct DataGrid;

    namespace implementation
    {
        struct DataGrid;
    }
}

namespace DataGridInternal
{

    class DataGridSelectedItemsCollection
    {
    public:
        DataGridSelectedItemsCollection(winrt::MiniDumpExplorer::implementation::DataGrid* owningGrid);
        ~DataGridSelectedItemsCollection();

        DataGridSelectedItemsCollection(DataGridSelectedItemsCollection const&) = delete;
        DataGridSelectedItemsCollection(DataGridSelectedItemsCollection &&) = delete;

        DataGridSelectedItemsCollection& operator=(DataGridSelectedItemsCollection const&) = delete;
        DataGridSelectedItemsCollection& operator=(DataGridSelectedItemsCollection &&) = delete;

        static constexpr auto InvalidIndex = int32_t{-1};

        [[nodiscard]] winrt::Windows::Foundation::IInspectable operator[](int32_t index) const;
        int32_t Add(winrt::Windows::Foundation::IInspectable const& dataItem) const;
        void Clear() const;
        [[nodiscard]] bool Contains(winrt::Windows::Foundation::IInspectable const& dataItem) const;
        [[nodiscard]] int32_t IndexOf(winrt::Windows::Foundation::IInspectable const& dataItem) const;
        void Remove(winrt::Windows::Foundation::IInspectable const& dataItem) const;
        void RemoveAt(int32_t index) const;
        [[nodiscard]] int32_t Count() const;

        [[nodiscard]] std::experimental::generator<winrt::Windows::Foundation::IInspectable> Items() const;

        [[nodiscard]] winrt::MiniDumpExplorer::DataGrid OwningGrid() const;
        [[nodiscard]] std::vector<winrt::Windows::Foundation::IInspectable> const& SelectedItemsCache() const;
        [[nodiscard]] std::vector<winrt::Windows::Foundation::IInspectable>& SelectedItemsCache();
        void ClearRows();
        [[nodiscard]] bool ContainsSlot(int32_t slot) const;
        [[nodiscard]] bool ContainsAll(int32_t startSlot, int32_t endSlot) const;
        void Delete(int32_t slot, winrt::Windows::Foundation::IInspectable const& item);
        void DeleteSlot(int32_t slot);
        [[nodiscard]] int32_t GetIndexCount(int32_t lowerBound, int32_t upperBound) const;
        [[nodiscard]] std::experimental::generator<int32_t> GetIndexes() const;
        [[nodiscard]] std::experimental::generator<int32_t> GetSlots(int32_t startSlot) const;

        winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs GetSelectionChangedEventArgs();
        void InsertIndex(int32_t slot);
        void SelectSlot(int32_t slot, bool select);
        void SelectSlots(int32_t startSlot, int32_t endSlot, bool select);
        void UpdateIndexes();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}
