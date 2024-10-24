#include <iostream>
#include <exception>

#include <pgf/taskengine/TaskEngine.hpp>
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
try
{
    pg::foundation::TaskEngine taskEngine;

    // some simple tasks  to be executed in order
    taskEngine.addTask([]() { std::cout << "Hello, "; });
    taskEngine.addTask([]() { std::cout << "World!" << std::endl; });

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
