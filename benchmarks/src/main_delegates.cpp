#include "ExecutionGraph/Common/TypeDefs.hpp"
#include "ExecutionGraph/Common/Delegates.hpp"

#include "BenchmarkConfig.hpp"
#include "BenchmarkFunctions.hpp"

#include <iostream>
#include <functional>
#include <vector>

using namespace execGraph;

using Type = std::vector<int>;
#define INIT_DATA \
    Type vec(3000); \
    int i = 0; \
    for(auto&v : vec) \
    { \
      v = ++i; \
    } \
    Type add(3000); \
    i = 0; \
    for(auto&v : add) \
    { \
      v = ++i*2; \
    }

template<typename T>
int rawFunction1(T& in, T& add)
{
  for( int i=0; i<in.size();++i)
  {
    in[i] += add[i];
  }
  return in[0];
}

struct A{
  A(){
    INIT_DATA
    m_vec = vec;
    m_add = add;
  }
  int compute()
  {
    for( int i=0; i<m_vec.size();++i)
    {
      m_vec[i] += m_add[i];
    }
    return m_vec[0];
  }
  Type m_vec, m_add;
};


static void BM_RawFunction1(benchmark::State &state) {

    INIT_DATA

    while (state.KeepRunning())
    {
        benchmark::DoNotOptimize( rawFunction1(vec,add) );
    }
}

static void BM_StdFunction1(benchmark::State &state) {
    INIT_DATA
    std::function<int(Type&,Type&)> t2 = &rawFunction1<Type>;
    while (state.KeepRunning())
    {
        t2(vec,add);
    }
}

static void BM_Delegate1(benchmark::State &state) {
    INIT_DATA
    Delegate<int(Type&,Type&)> t2 = &rawFunction1<Type>;
    while (state.KeepRunning())
    {
        t2(vec,add);
    }
}


static void BM_StdFunctionMember1(benchmark::State &state) {
    A a;
    std::function<int(void)> t2 = std::bind(&A::compute,a);
    while (state.KeepRunning())
    {
        benchmark::DoNotOptimize(t2());
    }
}

static void BM_DelegateMember1(benchmark::State &state) {
    A a;
    auto t2 = Delegate<int(void)>::from<A,&A::compute>(a);
    while (state.KeepRunning())
    {
        benchmark::DoNotOptimize(t2());
    }
}

// Register the function as a benchmark
BENCHMARK(BM_RawFunction1);
BENCHMARK(BM_StdFunction1);
BENCHMARK(BM_Delegate1);

BENCHMARK(BM_StdFunctionMember1);
BENCHMARK(BM_DelegateMember1);

BENCHMARK_MAIN();
