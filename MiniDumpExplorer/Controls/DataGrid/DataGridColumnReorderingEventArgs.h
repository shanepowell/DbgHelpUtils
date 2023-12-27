#pragma once

#include "DataGridColumnReorderingEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;

    struct DataGridColumnReorderingEventArgs : DataGridColumnReorderingEventArgsT<DataGridColumnReorderingEventArgs>
    {
        DataGridColumnReorderingEventArgs(MiniDumpExplorer::DataGridColumn const& dataGridColumn);

        MiniDumpExplorer::DataGridColumn Column() { return column_; }
        Microsoft::UI::Xaml::Controls::Control DragIndicator() const { return dragIndicator_; }
        void DragIndicator(Microsoft::UI::Xaml::Controls::Control const& value) { dragIndicator_ = value; }
        Microsoft::UI::Xaml::Controls::Control DropLocationIndicator() const { return dropLocationIndicator_; }
        void DropLocationIndicator(Microsoft::UI::Xaml::Controls::Control const& value) { dropLocationIndicator_ = value; }
        bool Cancel() const { return cancelled_; }
        void Cancel(bool const value) { cancelled_ = value; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
        Microsoft::UI::Xaml::Controls::Control dragIndicator_{nullptr};
        Microsoft::UI::Xaml::Controls::Control dropLocationIndicator_{nullptr};
        std::atomic_bool cancelled_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnReorderingEventArgs : DataGridColumnReorderingEventArgsT<DataGridColumnReorderingEventArgs, implementation::DataGridColumnReorderingEventArgs>
    {
    };
}
