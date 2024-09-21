#include <pgf/taskEngine/TaskEngine.hpp>
#include <thread>

void pgf::TaskEngine::run()
{
    while (!runner_thread.get_stop_token().stop_requested())
    {
        std::unique_lock lk(_mutex);
        _cv.wait(lk, [this] { return _task_available || runner_thread.get_stop_token().stop_requested(); });
        while (!_tasks.empty())
        {
            auto internal_task = std::move(_tasks.front());

            auto success = internal_task.execute();
            if (!success && internal_task.job.reschedule_on_failure)
            {
                _timed_tasks[std::chrono::high_resolution_clock::now() + internal_task.job.reschedule_delay] =
                    InternalTask{std::move(internal_task)};
            }
            _tasks.pop_front();
        }
        _task_available = false;

        lk.unlock();
        _cv.notify_all();
    }
}

void pgf::TaskEngine::stop()
{
    std::lock_guard lk(_mutex);
    _tasks.clear();
    _timed_tasks.clear();
    _task_available = false;
}

void pgf::TaskEngine::wait()
{
    // wait for all tasks to finish
    std::unique_lock lk(_mutex);
    _cv.wait(lk, [this] { return _timed_tasks.empty() && _tasks.empty(); });
}

void pgf::TaskEngine::checkTimedTasks()
{
    auto now = std::chrono::high_resolution_clock::now();
    checkTimedTasks(now);
}

void pgf::TaskEngine::checkTimedTasks(const Task::TimePoint& time)
{
    // get all delayed task with deadline passed
    {
        std::lock_guard lk(_mutex);
        auto            iter = _timed_tasks.lower_bound(time);
        // emplace all tasks from the map to the queue
        for (auto it = _timed_tasks.begin(); it != iter; it++)
        {
            _tasks.emplace_back(std::move(it->second));
        }
        // get number of tasks that are moved to the queue
        auto moved_tasks = std::distance(_timed_tasks.begin(), iter);
        _timed_tasks.erase(_timed_tasks.begin(), iter);
        _task_available = moved_tasks > 0;
        // if there are tasks available, notify the runner thread
    }
    if (_task_available) { _cv.notify_all(); }
}

pgf::TaskEngine::TaskEngine(Config&& config)
  : _config(config)
{
    if (_config.start_immediately) { start(); }
}

pgf::TaskEngine::~TaskEngine()
{
    stop();
    runner_thread.request_stop();
    _cv.notify_all();
    _check_thread.request_stop();
    _cv.notify_all();
    runner_thread.join();
    _check_thread.join();
}

void pgf::TaskEngine::addTask(Task&& task)
{
    addInternalTask(InternalTask{std::move(task)});
}

void pgf::TaskEngine::addInternalTask(InternalTask&& internal_task)
{
    auto            now = std::chrono::high_resolution_clock::now();
    std::lock_guard lk(_mutex);
    if (internal_task.job.starting_time_offset == std::chrono::high_resolution_clock::duration::zero())
    {
        _tasks.emplace_back(std::move(internal_task));
        _task_available = true;
        _cv.notify_one();
    }
    else { _timed_tasks[internal_task.job.starting_time_offset + now] = std::move(internal_task); }
}

void pgf::TaskEngine::forceCheckTimedTasks()
{
    // get all delayed task with deadline passed
    const std::lock_guard lk(_mutex);
    auto                  iter = _timed_tasks.lower_bound(Task::TimePoint::max());
    // emplace all tasks from the map to the queue
    for (auto it = _timed_tasks.begin(); it != iter; it++)
    {
        _tasks.emplace_back(std::move(it->second));
    }

    _timed_tasks.erase(_timed_tasks.begin(), iter);
    _task_available = true;
    _cv.notify_one();
}

bool pgf::TaskEngine::hasTimedTasks() const
{
    std::unique_lock lk(_mutex);
    return !_timed_tasks.empty();
}

void pgf::TaskEngine::addAsyncTask(Task&& task)
{
    InternalTask internal_task{std::move(task), true, _config.async_task_check_duration};
    addInternalTask(std::move(internal_task));
}

void pgf::TaskEngine::start()
{
    if (runner_thread.joinable()) { throw std::logic_error("TaskEngine is already running"); }
    runner_thread = std::jthread{&TaskEngine::run, this};
    if (_config.periodic_check_duration != std::chrono::high_resolution_clock::duration::zero())
    {
        _check_thread = std::jthread([this](std::stop_token stoken) {
            while (!stoken.stop_requested())
            {
                checkTimedTasks();
                std::this_thread::sleep_for(_config.periodic_check_duration);
            }
        });
    }
}
