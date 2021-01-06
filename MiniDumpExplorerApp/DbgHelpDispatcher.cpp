#include "pch.h"
#include "DbgHelpDispatcher.h"

DbgHelpDispatcher::DbgHelpDispatcher()
{
    thread_ = std::thread{[this]() { ProcessOperations(); }};
}

DbgHelpDispatcher::~DbgHelpDispatcher()
{
    shutdown_ = true;
    DoPost(std::function<void()>{});
    thread_.join();
}

void DbgHelpDispatcher::Post(std::function<void()> op)
{
    if(!op)
    {
        throw std::runtime_error("Invalid DbgHelpDispatcher Post");
    }

    DoPost(std::move(op));
}

void DbgHelpDispatcher::DoPost(std::function<void()> op)
{
    {
        std::unique_lock lock(mutex_);
        operations_.emplace(std::move(op));
    }
    cv_.notify_one();
}

void DbgHelpDispatcher::ProcessOperations()
{
    while(!shutdown_)
    {
        try
        {
            auto const op = GetNextOperation();

            if(!op)
            {
                return;
            }

            op();
        }
        catch (...)
        {
        }
    }
}

std::function<void()> DbgHelpDispatcher::GetNextOperation()
{
    std::unique_lock lock(mutex_);
    cv_.wait(lock);
    auto rv = operations_.top();
    operations_.pop();
    return rv;
}
