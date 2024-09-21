#include <iostream>
#include <exception>
#include <fmt/chrono.h>
#include <pgf/taskengine/TaskEngine.hpp>
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    auto            config = pgf::TaskEngine::default_config().withStartImmediately(false);
    pgf::TaskEngine taskEngine(std::move(config));
    taskEngine.addAsyncTask([]() { std::cout << "World!" << std::endl; }, false, std::chrono::milliseconds(6000));
    // an async task that returns false, so it will be rescheduled until a condition is met
    taskEngine.addAsyncTask(
        [count = 0]() mutable {
            count++;
            fmt::println("{:%Y-%m-%d %H:%M:%S} : Hello {}", std::chrono::system_clock::now(), count);
            return count == 5;
        },
        true,                             // reschedule on failure
        std::chrono::milliseconds(500),   // time to wait before starting the task
        std::chrono::milliseconds(1000)); // time to wait before rescheduling the task

    fmt::println("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());
    taskEngine.start();
    taskEngine.wait();
}
catch (const std::exception& e)
{
    // TODO: Log exception
    std::cerr << e.what() << std::endl;
}
catch (...)
{
    // TODO: Log unknown exception
    std::cerr << "Unknown exception" << std::endl;
}
