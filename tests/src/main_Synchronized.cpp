#include <thread>
#include "TestFunctions.hpp"
#include "executionGraph/common/Synchronized.hpp"

using namespace executionGraph;
using namespace std::chrono_literals;

struct A
{
    std::pair<double, double> get() const
    {
        return {a[0], a[1]};
    }
    void set(double v, double t)
    {
        a[0] = v;
        a[1] = t;
    }
    double a[2];
};

MY_TEST(Synchronized, SetterGetter)
{
    Synchronized<A> synced;

    std::thread a([&]() {
        DEFINE_RANDOM_GENERATOR_FUNC(1);
        for(int i = 0; i <= 1000; ++i)
        {
            auto sleep = int(rand() * 5);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
            //std::cout << "setting: " << i << std::endl;
            synced.wlock()->set(i, i);
        }
    });

    std::thread b([&]() {
        DEFINE_RANDOM_GENERATOR_FUNC(0);
        for(int i = 0; i <= 1000; ++i)
        {
            auto p     = synced.rlock()->get();
            auto sleep = int(rand() * 5);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));

            //std::cout << "p.first: " << p.first << std::endl;
            ASSERT_EQ(p.first, p.second);
        }
    });

    a.join();
    b.join();

    const Synchronized<A>& synced2 = synced;

    double r = synced2.withRLock([](auto& a) { return a.get().first; });
    ASSERT_EQ(r, 100);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
