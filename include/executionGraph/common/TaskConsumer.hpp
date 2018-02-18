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

#ifndef executionGraph_common_TaskConsumer_hpp
#define executionGraph_common_TaskConsumer_hpp

#include <atomic>
#include <memory>
#include <meta/meta.hpp>
#include <thread>
#include "executionGraph/common/SfinaeMacros.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        A task consumer thread which pops tasks from a thread-safe queue
        and executes them in its consumer loop.

        The tasks need a call operator `operator(std::thread:id id)`.

        @date Sun Feb 18 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TTaskQueue>
    class TaskConsumer
    {
    public:
        using TaskQueue = TTaskQueue;
        using Task      = typename TTaskQueue::Task;

    private:
        template<typename T>
        using IsSharedPointer = meta::is<T, std::shared_ptr>;

    public:
        TaskConsumer(std::shared_ptr<TaskQueue> queue)
            : m_queue(queue){};
        virtual ~TaskConsumer() = default;

        //! No move allowed!
        TaskConsumer(TaskConsumer&&) = delete;
        TaskConsumer& operator=(TaskConsumer&&) = delete;
        //! No copy allowed!
        TaskConsumer(const TaskConsumer&) = delete;
        TaskConsumer& operator=(const TaskConsumer&) = delete;

        //! Start this consumer.
        void start()
        {
            m_finished = false;
            m_thread   = std::thread([this]() { run(); });
        }

        //! Join this consumer (wait to stop it!)
        void join()
        {
            m_finished = true;
            if(m_thread.joinable())
            {
                m_thread.join();
            }
        }

        auto getId() { return m_thread.get_id(); }

    private:
        //! Consumer run loop.
        void run()
        {
            while(!m_finished)
            {
                auto optionalTask = m_queue->pop();
                if(optionalTask)
                {
                    runTask(*optionalTask, getId());  // run the task in this consumer thread.
                }
            }
        }

        //! Dispatching for std::shared_ptr
        template<typename T, typename... Args, EXECGRAPH_SFINAE_ENABLE_IF(IsSharedPointer<T>{})>
        void runTask(T& task, Args&&... args)
        {
            task->operator()(std::forward<Args>(args)...);
        }

        //! Dispatching for copyable
        template<typename T, typename... Args, EXECGRAPH_SFINAE_ENABLE_IF(!IsSharedPointer<T>{})>
        void runTask(T& task, Args&&... args)
        {
            task.operator()(std::forward<Args>(args)...);
        }

    private:
        std::atomic<bool> m_finished;        //!< Flag for terminating this consumer thread
        std::thread m_thread;                //!< The actual consumer thread.
        std::shared_ptr<TaskQueue> m_queue;  //!< The consumer task queue.
    };

}  // namespace executionGraph

#endif