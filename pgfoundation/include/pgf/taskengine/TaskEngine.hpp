#pragma once
#undef pgf
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <future>

namespace pg::foundation {
using namespace std::chrono_literals;

// a task to be executed by the tasked player, should be non-blocking and as fast as possible
struct Task
{
    using Duration = std::chrono::high_resolution_clock::duration;
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    // bool execute() { return task(); }

    std::function<bool()> task; //< the task needs to return true if it was successful, false otherwise.
    bool                  reschedule_on_failure = false; //< if the task fails, should it be rescheduled?
    Duration              starting_time_offset{0ms};     //< delay before executing the task
    Duration              reschedule_delay{0ms};         //< delay before rescheduling the task
};

struct InternalTask
{
    bool execute()
    {
        if (!async) { return job.task(); }
        else
        {
            // check if we already started the work
            if (!fut.valid())
            {
                // move the work to a shared future
                fut = std::async(std::launch::async, [work = std::move(job.task)]() {
                          // check the result of the work
                          bool res = work();
                          // if the job was successful, return true, and simply remove the task
                          if (res) { return std::make_pair(true, std::function<bool()>{}); }
                          // if the job failed, return false and the work to be rescheduled
                          else { return std::make_pair(false, std::move(work)); }
                      }).share();
            }
            // check if the task is done
            // give the thread some time to start
            if (fut.wait_for(async_check_duration) == std::future_status::ready)
            {
                auto res = fut.get();
                if (res.first)

                {
                    // we are done
                    return true;
                }
                else
                {
                    // reset the future, move the original task back in, so we can reschedule it if needed
                    job.task = std::move(res.second);
                    fut = {};
                }
            }

            return false;
        }
    }

    Task           job;
    bool           async = false;
    Task::Duration async_check_duration{1ms};

    std::shared_future<std::pair<bool, std::function<bool()>>> fut;
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
        Duration async_task_check_duration{1ms}; //< wait time for async tasks to be checked for completion.
        bool     start_immediately = true;       //< start the task engine immediately, else start() needs to be called

        // monadic
        Config& withPeriodicCheckDuration(Duration duration)
        {
            periodic_check_duration = duration;
            return *this;
        }

        Config& withAsyncTaskCheckDuration(Duration duration)
        {
            async_task_check_duration = duration;
            return *this;
        }

        Config& withStartImmediately(bool start)
        {
            start_immediately = start;
            return *this;
        }
    };

    static consteval Config default_config() { return Config{}; };

    TaskEngine(Config&& cfg = default_config());
    void start();

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

    // add a generic callable as a AsyncTask
    template <typename F>
    void addAsyncTask(F&&      f,
                      bool     reschedule_on_failure = false,
                      Duration starting_time_offset = {},
                      Duration reschedule_delay = {})
    {
        // if lambda's return type is void, we wrap it in a lambda that returns bool for convenience
        constexpr bool is_void = std::is_same_v<decltype(f()), void>;
        if constexpr (is_void)
        {
            addAsyncTask({[f = std::move(f)]() {
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
            addAsyncTask({std::move(f), reschedule_on_failure, starting_time_offset, reschedule_delay});
        }

        else { static_assert(is_void, "Task must return bool or void"); }
    }

    void addTask(Task&& task);

    void addAsyncTask(Task&& task);

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
    void addInternalTask(InternalTask&& task);
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

} // namespace pg::foundation