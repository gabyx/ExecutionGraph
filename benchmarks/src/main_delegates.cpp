// ========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#include "ExecutionGraph/common/Delegates.hpp"
#include "ExecutionGraph/common/TypeDefs.hpp"

#include <functional>
#include <iostream>
#include <vector>

#include "BenchmarkConfig.hpp"
#include "BenchmarkFunctions.hpp"

using namespace execGraph;

using Type = std::vector<int>;

static const int N = 10000;

template<typename T>
void initData(T& a, T& b)
{
    a.resize(3);
    int i = 0;
    for(auto& v : a)
    {
        v = ++i;
    }
    b.resize(3);
    for(auto& v : b)
    {
        v = ++i * 2;
    }
}

struct A;
struct B;
struct Base
{
    Base()
    {
        initData(m_vec, m_add);
    }
    int compute();

    Delegate<int(void)> computeByDelegate;
    std::function<int(void)> computeByStdFunction;

    int i = 0;
    Type m_vec, m_add;
};

struct A : Base
{
    A()
    {
        i = 0;
    }
    int compute()
    {
        for(int i = 0; i < m_vec.size(); ++i)
        {
            m_vec[i] += m_add[i];
        }
        return m_vec[0];
    }
};

struct B : Base
{
    B()
    {
        i = 1;
    }
    int compute()
    {
        for(int i = 0; i < m_vec.size(); ++i)
        {
            m_vec[i] += m_add[i];
        }
        return m_vec[0];
    }
};

int Base::compute()
{
    switch(i)
    {
        case 0:
            return static_cast<A*>(this)->compute();
        case 1:
            return static_cast<B*>(this)->compute();
    }
    return 0;
}

struct BaseVirt
{
    BaseVirt()
    {
        initData(m_vec, m_add);
    }
    virtual int compute() { return 0; }
    Type m_vec, m_add;
};

struct AVirt : BaseVirt
{
    virtual int compute() override
    {
        for(int i = 0; i < m_vec.size(); ++i)
        {
            m_vec[i] += m_add[i];
        }
        return m_vec[0];
    }
};

struct BVirt : BaseVirt
{
    virtual int compute() override
    {
        for(int i = 0; i < m_vec.size(); ++i)
        {
            m_vec[i] += m_add[i];
        }
        return m_vec[0];
    }
};

#define DO_NOT_OPTIMIZE  //benchmark::DoNotOptimize

static void BM_ByStaticCastToDerived(benchmark::State& state)
{
    std::vector<Base*> vec(N);
    for(int i = 0; i < N; ++i)
    {
        vec[i] = (i % 11 == 0 || i % 7 == 0) ? (Base*)new A() : (Base*)new B();
    }

    while(state.KeepRunning())
    {
        for(auto* v : vec)
        {
            DO_NOT_OPTIMIZE(v->compute());
        }
    }
}

static void BM_ByDelegate(benchmark::State& state)
{
    std::vector<Base*> vec(N);
    for(int i = 0; i < N; ++i)
    {
        if(i % 11 == 0 || i % 7 == 0)
        {
            A* a                 = new A;
            vec[i]               = a;
            a->computeByDelegate = Delegate<int(void)>::from<A, &A::compute>(a);
        }
        else
        {
            B* b                 = new B;
            vec[i]               = b;
            b->computeByDelegate = Delegate<int(void)>::from<B, &B::compute>(b);
        }
    }
    while(state.KeepRunning())
    {
        for(auto* v : vec)
        {
            DO_NOT_OPTIMIZE(v->computeByDelegate());
        }
    }
}

static void BM_ByStdFunction(benchmark::State& state)
{
    std::vector<Base*> vec(N);
    for(int i = 0; i < N; ++i)
    {
        if(i % 11 == 0 || i % 7 == 0)
        {
            A* a                    = new A;
            vec[i]                  = a;
            a->computeByStdFunction = [&]() { return a->compute(); };
        }
        else
        {
            B* b                    = new B;
            vec[i]                  = b;
            b->computeByStdFunction = [&]() { return b->compute(); };
        }
    }
    while(state.KeepRunning())
    {
        for(auto* v : vec)
        {
            DO_NOT_OPTIMIZE(v->computeByStdFunction());
        }
    }
}

static void BM_ByVirtual(benchmark::State& state)
{
    std::vector<BaseVirt*> vec(N);
    for(int i = 0; i < N; ++i)
    {
        vec[i] = (i % 11 == 0 || i % 7 == 0) ? (BaseVirt*)new AVirt() : (BaseVirt*)new BVirt();
    }
    while(state.KeepRunning())
    {
        for(auto* v : vec)
        {
            DO_NOT_OPTIMIZE(v->compute());
        }
    }
}

// Register the function as a benchmark
BENCHMARK(BM_ByStaticCastToDerived);
BENCHMARK(BM_ByDelegate);
BENCHMARK(BM_ByStdFunction);
BENCHMARK(BM_ByVirtual);

BENCHMARK_MAIN();
