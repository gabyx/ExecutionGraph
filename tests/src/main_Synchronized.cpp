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
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
