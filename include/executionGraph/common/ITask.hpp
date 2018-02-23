// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Feb 22 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_common_ITask_hpp
#define executionGraph_common_ITask_hpp

using executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
    Task-Interface which can be used for the class `ThreadPool`.
    Note that the task type for `ThreadPool` does not need to inherit from this class.
    It just needs the same functionality as this class.

    usage:

    @code
    struct MyTask : ITask { ... }
    ThreadPool<std::unique_ptr<ITask>> pool(4);
    pool.start();
    pool.getQueue()->emplace(std::make_unique<MyTask>());
    @endcode

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
    /* ---------------------------------------------------------------------------------------*/
    class ITask
    {
        //! No move/copy allowed!
        EXECGRAPH_DISALLOW_COPY_AND_MOVE(ITask)
    protected:
        ITask() = default;

    public:
        virtual ~ITask() = default;

    public:
        //! The call operator which executes the task in thread with `threadId`.
        virtual void runTask()(std::thread::id threadId) = 0;
        //! If an exception occures during `operator(...)` this call back is called
        //! in the same calling thread as `operator(...)`.
        virtual void onTaskException(const std::string& what) = 0;
    };
}
#endif
