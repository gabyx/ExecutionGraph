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
    Type vec(3); \
    int i = 0; \
    for(auto&v : vec) \
    { \
      v = ++i; \
    } \
    Type add(3); \
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

struct A;
struct B;
struct ABase{
  int compute();
  int computeByDelegate()
  {
    return del();
  }

  Delegate<int(void)> del;

  int i = 0;
};

struct A : ABase{
  A(){
    INIT_DATA
    m_vec = vec;
    m_add = add;
    i = 0;
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

struct B : ABase{
  B(){
    INIT_DATA
    m_vec = vec;
    m_add = add;
    i = 1;
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

int ABase::compute()
{
  switch(i){
  case 0:
    return static_cast<A*>(this)->compute();
    break;
  case 1:
    return static_cast<B*>(this)->compute();
    break;
  }
  return 0;
}

struct ABaseVirt{
  virtual int compute(){ return 0;}
};

struct ABaseVirt2 : ABaseVirt{
  virtual int compute(){ return 1;}
};

struct AVirt : ABaseVirt2{
  AVirt(){
    INIT_DATA
    m_vec = vec;
    m_add = add;
    i = 0;
  }
  virtual int compute() override
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
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize( rawFunction1(vec,add) );
      }
    }
}

static void BM_StdFunction1(benchmark::State &state) {
    INIT_DATA
    std::function<int(Type&,Type&)> t2 = &rawFunction1<Type>;
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        t2(vec,add);
      }
    }
}

static void BM_Delegate1(benchmark::State &state) {
    INIT_DATA
    Delegate<int(Type&,Type&)> t2 = &rawFunction1<Type>;
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        t2(vec,add);
      }
    }
}


static void BM_StdFunctionMember1(benchmark::State &state) {
    A a;
    std::function<int(void)> t2 = std::bind(&A::compute,a);
    while (state.KeepRunning())
    {
        for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(t2());
        }
    }
}

static void BM_DelegateMember1(benchmark::State &state) {
    A a;
    auto t2 = Delegate<int(void)>::from<A,&A::compute>(a);
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(t2());
      }
    }
}


static void BM_StaticCastToDerived(benchmark::State &state) {
    A a;
    ABase &aBase = a;
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(aBase.compute());
      }
    }
}

static void BM_ByStaticCastToDerived(benchmark::State &state) {
    A a;
    ABase &aBase = a;
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(aBase.compute());
      }
    }
}

static void BM_ByDelegate(benchmark::State &state) {
    A a;
    ABase &aBase = a;
    aBase.del = Delegate<int(void)>::from<A,&A::compute>(a);
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(aBase.computeByDelegate());
      }
    }
}

static void BM_ByVirtual(benchmark::State &state) {
    AVirt a;
    ABaseVirt &aBase = a;
    while (state.KeepRunning())
    {
      for(int i=1;i<100;i++){
        benchmark::DoNotOptimize(aBase.compute());
      }
    }
}




// Register the function as a benchmark
BENCHMARK(BM_RawFunction1);
BENCHMARK(BM_StdFunction1);
BENCHMARK(BM_Delegate1);

BENCHMARK(BM_StdFunctionMember1);
BENCHMARK(BM_DelegateMember1);

BENCHMARK(BM_ByStaticCastToDerived);
BENCHMARK(BM_ByDelegate);
BENCHMARK(BM_ByVirtual);


BENCHMARK_MAIN();
