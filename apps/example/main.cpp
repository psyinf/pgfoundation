#include <pgfoundation/taskengine.hpp>
#include <iostream>
#include <thread>
#include <future>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    pgf::TaskEngine taskEngine;
    //     auto            asyncTask = pgf::AsyncTask{[]() {
    //         std::cout << "starting async task" << std::endl;
    //         std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    //         std::cout << "Hello from thread!" << std::endl;
    //     }};
    auto task2 = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        std::cout << "done!" << std::endl;
    };

    // taskEngine.addTask(asyncTask, true, {}, {});
    /// taskEngine.addTask(task2, true, {}, {});
    taskEngine.addTask([]() { std::cout << "Hello, World!" << std::endl; });

    taskEngine.wait();

    // taskEngine.addTask([]() { std::chrono:: }) return 0;
}