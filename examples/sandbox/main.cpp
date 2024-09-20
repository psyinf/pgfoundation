#include <iostream>
#include <exception>

#include <pgf/taskengine/TaskEngine.hpp>
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pgf::TaskEngine taskEngine;

    auto task2 = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        std::cout << "done!" << std::endl;
    };
    taskEngine.addTask(std::move(task2), false, std::chrono::seconds(1), std::chrono::seconds(1));
    // taskEngine.addTask(asyncTask, true, {}, {});
    /// taskEngine.addTask(task2, true, {}, {});
    taskEngine.addTask([]() { std::cout << "Hello, World!" << std::endl; });

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
