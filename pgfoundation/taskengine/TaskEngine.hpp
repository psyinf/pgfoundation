#pragma once
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <iostream>

#include <future>

#include <set>

namespace pgf {
using namespace std::chrono_literals;

// a task to be executed by the tasked player, should be non-blocking and as fast as possible
struct Task
{
    using Duration = std::chrono::high_resolution_clock::duration;
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    bool execute() { return task(); }

    std::function<bool()> task; //< the task needs to return true if it was successful, false otherwise.
    bool                  reschedule_on_failure = false; //< if the task fails, should it be rescheduled?
    Duration              starting_time_offset{0ms};     //< delay before executing the task
    Duration              reschedule_delay{0ms};         //< delay before rescheduling the task
};

struct InternalTask
{
    Task task;

    std::future<void> fut;
};

// TODO: task encapsulation of a threaded task. Should hold a task, a future that is checked on the task function

/**
 * \brief A task engine that executes tasks in a separate thread.
 *
 * The pattern to be used is to add a non blocking task/fast finishing tasks. Tasks will be executed in order. Tasks
 * returning false will be rescheduled with a delay. Timed tasks will (re)executed after the specified time.
 * This was originally designed to be used in use with OpenAL, where the audio sources play asynchronously by
 * alPlay and finished playback can be checked by alGetSourcei with AL_SOURCE_STATE.
 * So this task engine is designed to be used with non-blocking tasks.
 */

class TaskEngine
{
public:
    using Duration = std::chrono::high_resolution_clock::duration;

    struct Config
    {
        using Duration = std::chrono::high_resolution_clock::duration;

        // periodically check for timed tasks. If set to 0, no periodic check will be performed and timed tasks will
        // only be handled by manually calling checkTimedTasks()
        Duration periodic_check_duration{16ms};
    };

    TaskEngine(Config&& cfg = {.periodic_check_duration{16ms}});
    ~TaskEngine();

    // add a generic callable as a Task
    template <typename F>
    void addTask(F&&      f,
                 bool     reschedule_on_failure = false,
                 Duration starting_time_offset = {},
                 Duration reschedule_delay = {})
    {
        // if lambda's return type is void, we wrap it in a lambda that returns bool for convenience
        constexpr bool is_void = std::is_same_v<decltype(f()), void>;
        if constexpr (is_void)
        {
            addTask({[f = std::move(f)]() {
                         f();
                         return true;
                     },
                     reschedule_on_failure,
                     starting_time_offset,
                     reschedule_delay});
        }
        // is convertible to bool
        else if constexpr (std::is_convertible_v<decltype(f()), bool>)
        {
            addTask({std::move(f), reschedule_on_failure, starting_time_offset, reschedule_delay});
        }

        else { static_assert(is_void, "Task must return bool or void"); }
    }

    void addTask(Task&& task);

    void addAsyncTask(Task&& task)
    {
        auto fut = std::async(std::launch::deferred, [task = std::move(task)]() mutable { task.execute(); });

        // _async_tasks.emplace(std::move(task));
    }

    // check for task's deadline and move them to the task queue
    void checkTimedTasks();
    // forced run of checkTimedTasks
    void forceCheckTimedTasks();

    // wait for all tasks to finish
    void wait();
    // stop the player
    void stop();
    // check if there are any tasks available
    bool hasTimedTasks() const;

private:
    void checkTimedTasks(const Task::TimePoint& time);
    void run();

    mutable std::mutex                      _mutex;
    std::condition_variable                 _cv; //< used to notify the engine that a new task is available
    bool                                    _task_available = false; //< flag to signal that a new task is available
    std::deque<InternalTask>                _tasks;                  //< synchronous tasks to be executed
    std::map<Task::TimePoint, InternalTask> _timed_tasks;            //< tasks to be executed at a specific time
    Config                                  _config{};
    std::jthread                            runner_thread;
    std::jthread                            _check_thread;
}; // namespace pgf

} // namespace pgf