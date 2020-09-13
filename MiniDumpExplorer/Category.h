#pragma once

#include "Category.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Category : CategoryT<Category>
    {
        Category();
        Category(hstring name,
                 hstring category_icon,
                 Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> children,
                 bool is_leaf = false);

        hstring Name() const;
        void Name(hstring value);
        hstring CategoryIcon() const;
        void CategoryIcon(hstring value);
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> Children() const;
        void Children(Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> value);
        bool IsLeaf() const;
        void IsLeaf(bool value);

    private:
        hstring name_;
        hstring category_icon_;
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> children_;
        bool is_leaf_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Category : CategoryT<Category, implementation::Category>
    {
    };
}
