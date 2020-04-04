#include <thread>
#include "TestFunctions.hpp"
#include "executionGraph/common/Synchronized.hpp"

using namespace executionGraph;
using namespace std::chrono_literals;

struct A
{
    A(std::function<double()> randIn)
        : rand(std::move(randIn))
    {}
    std::pair<double, double> get() const
    {
        auto x = a[0];
        //std::cout << "getting: " << x << std::endl;
        auto sleep = int(rand() * 5);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        auto y = a[1];
        return {x, y};
    }
    void set(double v, double t)
    {
        //std::cout << "setting: " << v << std::endl;
        a[0]       = v;
        auto sleep = int(rand() * 5);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        a[1] = t;
    }
    double a[2];
    std::function<double()> rand;
};

EG_TEST(Synchronized, SetterGetter)
{
    DEFINE_RANDOM_GENERATOR_FUNC(1);
    Synchronized<A> synced{rand};

    std::thread a([&]() {
        for(int i = 0; i <= 100; ++i)
        {
            synced.wlock()->set(i, i);
        }
    });

    std::thread b([&]() {
        for(int i = 0; i <= 100; ++i)
        {
            auto p = synced.rlock()->get();
            ASSERT_EQ(p.first, p.second);
        }
    });

    a.join();
    b.join();

    const Synchronized<A>& synced2 = synced;

    double r = synced2.withRLock([](auto& a) { return a.get().first; });
    ASSERT_EQ(r, 100);
}

EG_TEST(Synchronized, SetterGetterDefect)
{
    DEFINE_RANDOM_GENERATOR_FUNC(1);

    struct DefectMutex
    {
        void lock() {}
        void unlock(){};
        void try_lock(){};
    };

    Synchronized<A, DefectMutex> synced{rand};
    bool failed = false;
    std::thread a([&]() {
        for(int i = 0; i <= 100; ++i)
        {
            synced.wlock()->set(i, i);
        }
    });

    std::thread b([&]() {
        for(int i = 0; i <= 100; ++i)
        {
            auto p = synced.rlock()->get();
            failed |= p.first != p.second;
        }
    });

    a.join();
    b.join();

    ASSERT_TRUE(failed) << "Synchronized with a NoMutex should fail!";
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
