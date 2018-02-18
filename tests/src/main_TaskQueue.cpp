//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 16 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include <array>
#include "TestFunctions.hpp"
#include "executionGraph/common/IObjectID.hpp"
#include "executionGraph/common/TaskConsumer.hpp"
#include "executionGraph/common/TaskQueue.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-vtables"
#endif

using namespace executionGraph;
using namespace std::chrono_literals;

class PrintThread : public std::ostringstream
{
public:
    PrintThread() = default;
    ~PrintThread()
    {
        std::lock_guard<std::mutex> guard(s_mutex);
        std::cout << this->str();
    }

private:
    static std::mutex s_mutex;
};

std::mutex PrintThread::s_mutex{};

struct Task
{
    EXECGRAPH_OBJECT_ID_NON_VIRTUAL_DECLARATION

public:
    Task(int i)
        : m_i(i) {}
    ~Task()
    {
        //PrintThread{} << "Task: " << m_i << " dtor " << std::endl;
    }
    Task(Task&& t)
    {
        m_i   = t.m_i;
        t.m_i = -1;
        m_a   = std::move(t.m_a);
    };
    Task& operator=(Task&& t)
    {
        m_i   = t.m_i;
        t.m_i = -1;
        m_a   = std::move(t.m_a);
        return *this;
    };

    void operator()(std::thread::id threadId)
    {
        EXECGRAPH_THROW_EXCEPTION_IF(m_i < 0, "Wow, a moved task gets executed!! WTF!");
        std::string id = getId();
        DEFINE_RANDOM_GENERATOR_FUNC(hashString(id));
        using namespace std::chrono_literals;
        auto sleep = int(rand() * 2000);
        PrintThread{} << "Thread: " << threadId << " Task: " << m_i << " running: " << sleep << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        PrintThread{} << "Thread: " << threadId << " Task: " << m_i << " finished " << std::endl;
    }
    int m_i;
    std::unique_ptr<int> m_a;  // testing movable tasks
};

MY_TEST(ProducerConsumer, Test1)
{
    DEFINE_RANDOM_GENERATOR_FUNC(hashString("Test1"))

    using TaskQueue                   = TaskQueue<Task>;
    using Consumer                    = TaskConsumer<TaskQueue>;
    auto queue                        = std::make_shared<TaskQueue>();
    std::array<Consumer, 8> consumers = {Consumer(queue), Consumer(queue), Consumer(queue), Consumer(queue), Consumer(queue), Consumer(queue), Consumer(queue), Consumer(queue)};

    for(auto& cons : consumers)
    {
        cons.start();
    }

    for(int l = 0; l < 3; ++l)
    {
        PrintThread{} << "Pushing tasks ..." << std::endl;

        for(int i = 0; i < 20; ++i)
        {
            PrintThread{} << "task " << i << " adding " << std::endl;
            Task t{i};
            queue->emplace(std::move(t));
            std::this_thread::sleep_for(std::chrono::milliseconds(int(rand() * 200)));
        }

        auto sleep = int(rand() * 2000);
        PrintThread{} << "Going to sleep: " << sleep << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }

    for(auto& c : consumers)
    {
        c.join();
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
