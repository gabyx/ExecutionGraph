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

#pragma once

#include <mutex>
#include "executionGraph/common/TaskConsumer.hpp"
#include "executionGraph/common/TaskQueue.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        Simple Thread-Pool.

        @date Thu Feb 22 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TTask>
    class ThreadPool
    {
        //! No move/copy allowed!
        EG_DISALLOW_COPY_AND_MOVE(ThreadPool)

    public:
        using Task     = TTask;
        using Queue    = TaskQueue<TTask>;
        using Consumer = TaskConsumer<Queue>;

    public:
        ThreadPool(std::size_t nThreads)
            : m_queue(std::make_shared<Queue>())
        {
            for(auto i = 0u; i < nThreads; ++i)
            {
                m_consumers.emplace_back(std::make_unique<Consumer>(m_queue));
            }
        };

        //! Destructor joins all threads automatically by the DTOR of Consumer.
        virtual ~ThreadPool() = default;

        //! Start all threads.
        void start()
        {
            std::scoped_lock<std::mutex> lock(m_access);
            for(auto& consumer : m_consumers)
            {
                consumer->start();
            }
        }

        //! Join all threads.
        void join()
        {
            std::scoped_lock<std::mutex> lock(m_access);
            for(auto& consumer : m_consumers)
            {
                consumer->join();
            }
        }

        //! Return the thread-safe queue into which task can be placed.
        const std::shared_ptr<Queue>& getQueue() { return m_queue; }

    private:
        std::shared_ptr<Queue> m_queue;  //!< The thread-safe task queue.

    private:
        std::vector<std::unique_ptr<Consumer>> m_consumers;  //!< All consumer threads.
        std::mutex m_access;                                 //!< Access mutex for this pool.
    };
}  // namespace executionGraph
