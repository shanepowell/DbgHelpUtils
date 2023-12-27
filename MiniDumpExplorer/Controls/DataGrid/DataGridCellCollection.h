#pragma once

namespace winrt::MiniDumpExplorer
{
    struct DataGridCell;
    struct DataGridCellEventArgs;
    struct DataGridRow;

    namespace implementation
    {
        struct DataGridRow;
    }
}

namespace DataGridInternal
{
    class DataGridCellCollection
    {
    public:
        DataGridCellCollection(winrt::MiniDumpExplorer::implementation::DataGridRow* owningRow);
        ~DataGridCellCollection();

        DataGridCellCollection(DataGridCellCollection const&) = delete;
        DataGridCellCollection(DataGridCellCollection &&) = default;

        DataGridCellCollection& operator=(DataGridCellCollection const&) = delete;
        DataGridCellCollection& operator=(DataGridCellCollection &&) = default;

        winrt::event_token CellAdded(winrt::Windows::Foundation::EventHandler<winrt::MiniDumpExplorer::DataGridCellEventArgs> const& value);
        void CellAdded(winrt::event_token const& value);

        winrt::event_token CellRemoved(winrt::Windows::Foundation::EventHandler<winrt::MiniDumpExplorer::DataGridCellEventArgs> const& value);
        void CellRemoved(winrt::event_token const& value);

        size_t Count() const;
        std::vector<winrt::MiniDumpExplorer::DataGridCell> const& GetCells() const;

        void Insert(size_t cellIndex, winrt::MiniDumpExplorer::DataGridCell const& cell);
        void RemoveAt(size_t cellIndex);

        winrt::MiniDumpExplorer::DataGridCell operator[](size_t index) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}

