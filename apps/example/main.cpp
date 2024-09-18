#include <pfoundation/TaskEngine.hpp>
#include <iostream>
#include <thread>
#include <future>

struct AsyncTask
{
    AsyncTask(std::function<void()> f)
      : _future(std::async(std::launch::async, f))

    {
    }

    AsyncTask(AsyncTask&& other)
      : _future(std::move(other._future))
    {
    }

    // move assignment operator
    AsyncTask& operator=(AsyncTask&& other)
    {
        if (this != &other) { _future = std::move(other._future); }
        return *this;
    }

    bool operator()() { return _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

    std::future<void> _future;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    pfoundation::TaskEngine taskEngine;
    auto asyncTask = AsyncTask{[]() { std::this_thread::sleep_for(std::chrono::milliseconds(500)); }};
    auto task2 = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "done!" << std::endl;
    };
    taskEngine.addTask(std::move(task2), false, std::chrono::seconds(1), std::chrono::seconds(1));

    taskEngine.wait();

    // taskEngine.addTask([]() { std::chrono:: }) return 0;
}