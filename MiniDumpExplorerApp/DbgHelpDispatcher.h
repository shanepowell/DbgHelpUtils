#pragma once
#include <thread>
#include <mutex>
#include <stack>
#include <functional>

class DbgHelpDispatcher
{
public:
    DbgHelpDispatcher();
    ~DbgHelpDispatcher();

    DbgHelpDispatcher(DbgHelpDispatcher const&) = delete;
    DbgHelpDispatcher(DbgHelpDispatcher &&) = delete;

    DbgHelpDispatcher& operator=(DbgHelpDispatcher const&) = delete;
    DbgHelpDispatcher& operator=(DbgHelpDispatcher &&) = delete;

    void Post(std::function<void()> op);

private:
    void DoPost(std::function<void()> op);
    void ProcessOperations();
    std::function<void()> GetNextOperation();

private:
    std::atomic_bool shutdown_{false};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::stack<std::function<void()>> operations_;
};
