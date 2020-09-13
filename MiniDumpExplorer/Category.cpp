#include "pch.h"
#include "Category.h"
#if __has_include("Category.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Category.g.cpp"
#endif

namespace winrt::MiniDumpExplorer::implementation
{
    Category::Category()
        : children_{winrt::single_threaded_observable_vector<MiniDumpExplorer::Category>()}
    {
    }

    Category::Category(hstring name,
                       hstring category_icon,
                       Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> children,
                       bool const is_leaf)
        : name_(std::move(name))
          , category_icon_(std::move(category_icon))
          , children_(std::move(children))
          , is_leaf_(is_leaf)
    {
    }

    hstring Category::Name() const
    {
        return name_;
    }

    void Category::Name(hstring value)
    {
        name_ = std::move(value);
    }

    hstring Category::CategoryIcon() const
    {
        return category_icon_;
    }

    void Category::CategoryIcon(hstring value)
    {
        category_icon_ = std::move(value);
    }

    Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> Category::Children() const
    {
        return children_;
    }

    void Category::Children(Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::Category> value)
    {
        children_ = std::move(value);
    }

    bool Category::IsLeaf() const
    {
        return is_leaf_;
    }

    void Category::IsLeaf(bool const value)
    {
        is_leaf_ = value;
    }
}
