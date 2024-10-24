#include <iostream>
#include <exception>

#include <pgf/taskengine/TaskEngine.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::foundation::TaskEngine taskEngine;

    // some task that returns false, so it will be rescheduled until a condition is met
    taskEngine.addTask(
        []() mutable {
            static const auto start_time = std::chrono::high_resolution_clock::now();
            auto              now = std::chrono::high_resolution_clock::now();
            static int        count = 0;
            if (now - start_time < std::chrono::seconds(1))
            {
                count++;
                std::cout << ".";
                return false;
            }
            std::cout << count << " done\n";
            return true;
        },
        true,                            // reschedule on failure
        std::chrono::milliseconds(1000), // time to wait before starting the task
        std::chrono::milliseconds(0));   // time to wait before rescheduling the task

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
