
// ========================================================================================
//  executionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// ========================================================================================

#ifndef ExecutionGraph_Common_Delegates_HPP
#define ExecutionGraph_Common_Delegates_HPP

#include <cassert>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "ExecutionGraph/common/SfinaeMacros.hpp"

namespace execGraph
{
template<typename T>
class Delegate;

template<class R, class... Args>
class Delegate<R(Args...)>
{
private:
    using FunctionType = R (*)(Args...);
    using InvokerType  = R (*)(void*, Args&&...);
    template<typename C>
    using MemFunctionType = R (C::*)(Args...);
    template<typename C>
    using CMemFunctionType = R (C::*)(Args...) const;

    //! Two Argument Constructors ============================================
private:
    Delegate(void* const pObject, InvokerType const functionPtr) noexcept
        : m_pObject(pObject), m_invoker(functionPtr)
    {
    }

public:
    Delegate()                = default;
    Delegate(Delegate const&) = default;
    Delegate(Delegate&&)      = default;
    Delegate(std::nullptr_t const) noexcept
        : Delegate() {}

    template<typename C>
    Delegate(C* const pObject, MemFunctionType<C> const methodPtr)
    {
        *this = from(pObject, methodPtr);
    }

    template<typename C>
    Delegate(C* const pObject, CMemFunctionType<C> const methodPtr)
    {
        *this = from(pObject, methodPtr);
    }

    template<typename C>
    Delegate(C& object, MemFunctionType<C> const methodPtr)
    {
        *this = from(object, methodPtr);
    }

    template<typename C>
    Delegate(C const& object, CMemFunctionType<C> const methodPtr)
    {
        *this = from(object, methodPtr);
    }

    //! One Argument Constructor ==============================================
    template<typename T, EXECGRAPH_SFINAE_ENABLE_IF((!std::is_same<Delegate, std::decay<T>>::value))>
    Delegate(T&& f)
        : m_functorStorage(operator new(sizeof(std::decay_t<T>)), functorDeleter<std::decay_t<T>>)
        , m_functorStorage_size(sizeof(std::decay_t<T>))
    {
        using FunctorType = std::decay_t<T>;
        new(m_functorStorage.get()) FunctorType(std::forward<T>(f));
        m_pObject = m_functorStorage.get();
        m_invoker = functorStub<FunctorType>;
        m_deleter = deleterStub<FunctorType>;
    }

    Delegate& operator=(Delegate const&) = default;
    Delegate& operator=(Delegate&&) = default;

    template<typename C>
    Delegate& operator=(MemFunctionType<C> const rhs)
    {
        return *this = from(static_cast<C*>(m_pObject), rhs);
    }

    template<typename C>
    Delegate& operator=(CMemFunctionType<C> const rhs)
    {
        return *this = from(static_cast<C const*>(m_pObject), rhs);
    }

    template<typename T, EXECGRAPH_SFINAE_ENABLE_IF((!std::is_same<Delegate, typename std::decay_t<T>>::value))>
    Delegate& operator=(T&& f)
    {
        using FunctorType = typename std::decay_t<T>;

        if((sizeof(FunctorType) > m_functorStorage_size) || !m_functorStorage.unique())
        {
            m_functorStorage.reset(operator new(sizeof(FunctorType)), functorDeleter<FunctorType>);

            m_functorStorage_size = sizeof(FunctorType);
        }
        else
        {
            m_deleter(m_functorStorage.get());
        }
        new(m_functorStorage.get()) FunctorType(std::forward<T>(f));
        m_pObject = m_functorStorage.get();
        m_invoker = functorStub<FunctorType>;
        m_deleter = deleterStub<FunctorType>;
        return *this;
    }

    template<FunctionType FunctionPtr>
    static Delegate from() noexcept
    {
        return {nullptr, &function_stub<FunctionPtr>};
    }

    //! from<Args,&Args::foo> ( obj ) intializers
    //! ===================================
    template<typename C, MemFunctionType<C> MemberPtr>
    static Delegate from(C* const pObject) noexcept
    {
        return {pObject, &method_stub<C, MemberPtr>};
    }

    template<typename C, CMemFunctionType<C> MemberPtr>
    static Delegate from(C const* const pObject) noexcept
    {
        return {const_cast<C*>(pObject), &const_method_stub<C, MemberPtr>};
    }

    template<typename C, MemFunctionType<C> MemberPtr>
    static Delegate from(C& object) noexcept
    {
        return {&object, &method_stub<C, MemberPtr>};
    }

    template<typename C, CMemFunctionType<C> MemberPtr>
    static Delegate from(C const& object) noexcept
    {
        return {const_cast<C*>(&object), &const_method_stub<C, MemberPtr>};
    }
    // ===========================================================================

    template<typename T>
    static Delegate from(T&& f)
    {
        return std::forward<T>(f);
    }
    static Delegate from(FunctionType functionPtr) { return {nullptr, &functionPtr}; }

    template<typename C>
    using MemberPair = std::pair<C* const, MemFunctionType<C> const>;
    template<typename C>
    using MemberPairConst = std::pair<C const* const, CMemFunctionType<C> const>;

    //  //! from( obj, memberFuncPtr ) intializers
    //  ===================================
    //  template <typename C>
    //  static Delegate from(C* const pObject, MemFunctionType<C> const
    //  methodFuncPtr)
    //  {
    //    return from<C,decltype(methodFuncPtr)>(pObject);
    //  }
    //
    //  template <typename C>
    //  static Delegate from(C const* const pObject, CMemFunctionType<C> const
    //  methodFuncPtr)
    //  {
    //    return from<C,decltype(methodFuncPtr)>(pObject);
    //  }
    //
    //
    //  template <typename C>
    //  static Delegate from(C& object, MemFunctionType<C> const methodFuncPtr)
    //  {
    //    return from<C, MemFunctionType<C> >(object);
    //  }
    //
    //
    //  template <typename C>
    //  static Delegate from(C const& object, ConstMemberFuncPtr<C> const
    //  methodFuncPtr)
    //  {
    //    return from<C,decltype(methodFuncPtr)>(object);
    //  }
    // ===========================================================================

    void swap(Delegate& other) noexcept { std::swap(*this, other); }

    bool operator==(Delegate const& rhs) const noexcept
    {
        return (m_pObject == rhs.m_pObject) && (m_invoker == rhs.m_invoker);
    }
    bool operator!=(Delegate const& rhs) const noexcept { return !operator==(rhs); }

    bool operator==(std::nullptr_t const) const noexcept { return m_invoker == nullptr; }
    bool operator!=(std::nullptr_t const) const noexcept { return m_invoker != nullptr; }
    explicit operator bool() const noexcept { return m_invoker; }

    //! Execute the Delegate.
    R operator()(Args... args) const { return m_invoker(m_pObject, std::forward<Args>(args)...); }

private:
    friend struct std::hash<Delegate>;

    void* m_pObject       = nullptr;  //!< Type-Erased Object (if functor or member function)
    InvokerType m_invoker = nullptr;  //!< Function pointer

    /** Functor Storage */
    std::shared_ptr<void> m_functorStorage;
    std::size_t m_functorStorage_size;

    /**! Deleter for std::shared_ptr m_functorStorage */
    template<class T>
    static void functorDeleter(void* const p)
    {
        static_cast<T*>(p)->~T();
        operator delete(p);
    }

    using DeleterFunctionPtr = void (*)(void*);
    DeleterFunctionPtr m_deleter;  //!< Type-Erased Deleter which only calls dtor
                                   //! of the object inside m_functorStorage
    template<class T>
    static void deleterStub(void* const p)
    {
        static_cast<T*>(p)->~T();
    }

    //! Called function for raw function pointers
    template<FunctionType FunctionPtr>
    static R function_stub(void* const, Args&&... args)
    {
        return FunctionPtr(std::forward<Args>(args)...);
    }

    //! Called function for member function pointers
    template<typename C, MemFunctionType<C> MethodPtr>
    static R method_stub(void* const pObject, Args&&... args)
    {
        return (static_cast<C*>(pObject)->*MethodPtr)(std::forward<Args>(args)...);
    }

    //! Called function for const member function pointers
    template<typename C, CMemFunctionType<C> MethodPtr>
    static R const_method_stub(void* const pObject, Args&&... args)
    {
        return (static_cast<C const*>(pObject)->*MethodPtr)(std::forward<Args>(args)...);
    }

    template<typename>
    struct is_MemberPair : std::false_type
    {
    };
    template<typename C>
    struct is_MemberPair<std::pair<C* const, R (C::*const)(Args...)>> : std::true_type
    {
    };

    template<typename>
    struct is_MemberPairConst : std::false_type
    {
    };
    template<typename C>
    struct is_MemberPairConst<std::pair<C const* const, R (C::*const)(Args...) const>> : std::true_type
    {
    };

    //! Called function for functor types
    template<typename T>
    static typename std::enable_if_t<!(is_MemberPair<T>{} || is_MemberPairConst<T>{}), R> functorStub(
        void* const pObject, Args&&... args)
    {
        return (*static_cast<T*>(pObject))(std::forward<Args>(args)...);
    }

    //! Called function for functor types
    template<typename T>
    static typename std::enable_if_t<is_MemberPair<T>{} || is_MemberPairConst<T>{}, R> functorStub(void* const pObject,
                                                                                                   Args&&... args)
    {
        return (static_cast<T*>(pObject)->first->*static_cast<T*>(pObject)->second)(std::forward<Args>(args)...);
    }
};
}

namespace std
{
template<typename R, typename... Args>
struct hash<execGraph::Delegate<R(Args...)>>
{
    size_t operator()(execGraph::Delegate<R(Args...)> const& d) const noexcept
    {
        auto const seed(hash<void*>()(d.m_pObject));

        return hash<typename execGraph::Delegate<R(Args...)>::FunctionType>()(d.m_invoker) + 0x9e3779b9 + (seed << 6) +
               (seed >> 2);
    }
};
}
#endif
