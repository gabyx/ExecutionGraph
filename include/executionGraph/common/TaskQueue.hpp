//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Sun Feb 18 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_common_TaskQueue_hpp
#define executionGraph_common_TaskQueue_hpp

#include <chrono>
#include <mutex>
#include <optional>
#include <queue>

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        A thread-safe task queue.
        The tasks which are pushed into this queue are copied.
        So the TTask type either is cheap copyable or movable or is a std::shared_ptr<T>.

        @date Sun Feb 18 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TTask>
    class TaskQueue
    {
    public:
        using Task = TTask;

    public:
        TaskQueue()          = default;
        virtual ~TaskQueue() = default;

        //! Move allowed!
        TaskQueue(TaskQueue&&) = default;
        TaskQueue& operator=(TaskQueue&&) = default;

        //! No copy allowed!
        TaskQueue(const TaskQueue&) = delete;
        TaskQueue& operator=(const TaskQueue&) = delete;

    public:
        template<typename... Args>
        void emplace(Args&&... args)
        {
            // lock the queue
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace(std::forward<Args>(args)...);  // forward all args to the constructor of the Task
            // unlock the queue
            lock.unlock();

            // notify some other thread
            m_cond.notify_one();
        }

        //! Get a next task. Block for `blockTime`.
        std::optional<Task> pop(const std::chrono::milliseconds blockTime = std::chrono::milliseconds(200))
        {
            // lock the queue (dtor unlocks)
            std::unique_lock<std::mutex> lock(m_mutex);
            // releases `lock`, waits until awakend or timeOut (lambda is for spurious wake up)
            if(m_cond.wait_for(lock, blockTime, [this]() { return !m_queue.empty(); }))
            {
                Task task = std::move(m_queue.front());  // `lock` is locked
                m_queue.pop();
                return task;
            }
            return {};
        }

    private:
        std::queue<Task> m_queue;        //!< The queue with all work items.
        std::mutex m_mutex;              //!< Mutex to lock the queue.
        std::condition_variable m_cond;  //!< Worker wait on this condition variable.
    };
}  // namespace executionGraph

#endif