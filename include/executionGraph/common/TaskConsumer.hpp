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
#include "executionGraph/common/AccessMacros.hpp"
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
        //! No move/copy allowed!
        EXECGRAPH_DISALLOW_COPY_AND_MOVE(TaskConsumer)

    public:
        using TaskQueue = TTaskQueue;
        using Task      = typename TTaskQueue::Task;

    private:
        template<typename T>
        using IsPointerType = meta::or_<meta::is<T, std::shared_ptr>, meta::is<T, std::unique_ptr>>;

    public:
        TaskConsumer(const std::shared_ptr<TaskQueue>& queue)
            : m_queue(queue){};
        virtual ~TaskConsumer() { join(); };

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
        template<typename T, typename = void>
        struct Dispatch;

        //! Dispatching for std::shared_ptr.
        template<typename T>
        struct Dispatch<T, EXECGRAPH_SFINAE_ENABLE_IF_CLASS(IsPointerType<T>{})>
        {
            template<typename... Args>
            static void runTask(T& task, Args&&... args)
            {
                task->runTask(std::forward<Args>(args)...);
            }
            template<typename... Args>
            static void onTaskException(T& task, Args&&... args)
            {
                task->onTaskException(std::forward<Args>(args)...);
            }
        };

        //! Dispatching for normal types.
        template<typename T>
        struct Dispatch<T, EXECGRAPH_SFINAE_ENABLE_IF_CLASS(!IsPointerType<T>{})>
        {
            template<typename... Args>
            static void runTask(T& task, Args&&... args)
            {
                task.runTask(std::forward<Args>(args)...);
            }
            template<typename... Args>
            static void onTaskException(T& task, Args&&... args)
            {
                task.onTaskException(std::forward<Args>(args)...);
            }
        };

        //! Consumer run loop.
        void run()
        {
            while(!m_finished)
            {
                auto optionalTask = m_queue->pop();
                if(optionalTask)
                {
                    try
                    {
                        Dispatch<Task>::runTask(*optionalTask, getId());  // run the task in this consumer thread.
                    }
                    catch(const std::exception& e)
                    {
                        Dispatch<Task>::onTaskException(*optionalTask, e.what());
                    }
                    catch(...)
                    {
                        Dispatch<Task>::onTaskException(*optionalTask, "An unknown unhandled exception occured!");
                    }
                }
            }
        }

    private:
        std::atomic<bool> m_finished;        //!< Flag for terminating this consumer thread
        std::thread m_thread;                //!< The actual consumer thread.
        std::shared_ptr<TaskQueue> m_queue;  //!< The consumer task queue.
    };

}  // namespace executionGraph

#endif