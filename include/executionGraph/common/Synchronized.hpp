// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Aug 21 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_common_Synchronized_hpp
#define executionGraph_common_Synchronized_hpp

#include <memory>
#include <mutex>

namespace executionGraph
{
    namespace details
    {
        template<typename T>
        T exchange(T& value, T&& newValue)
        {
            T oldValue{value};
            value = std::forward(newValue);
            return oldValue;
        }

        /* ---------------------------------------------------------------------------------------*/
        /*!
            The auto lock functionality for the Sychronized class.

            @date Fri Aug 24 2018
            @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
        */
        /* ---------------------------------------------------------------------------------------*/

        template<typename TLock, typename TTSynchronized>
        class LockedPtr
        {
        public:
            using LockType          = TLock;
            using CSynchronizedType = TTSynchronized;
            using SynchronizedType  = std::remove_const_t<TTSynchronized>;
            using CDataType         = std::conditional_t<std::is_const_v<CSynchronizedType>,
                                                 const typename SynchronizedType::Data,
                                                 typename SynchronizedType::Data>;

        private:
            friend SynchronizedType;

        public:
            /*
            * Copy constructor and assignment operator are deleted.
            */
            LockedPtr(const LockedPtr& rhs) = delete;
            LockedPtr& operator=(const LockedPtr& rhs) = delete;

            /**
             * Check if this LockedPtr is uninitialized, or points to valid locked data.
             *
             * This method can be used to check if a timed-acquire operation succeeded.
             * If an acquire operation times out it will result in a null LockedPtr.
             *
             * A LockedPtr is always either null, or holds a lock to valid data.
             */
            bool isNull() const
            {
                return this->m_parent == nullptr;
            }

            /**
             * Explicit boolean conversion.
             *
             * Returns !isNull()
             */
            explicit operator bool() const
            {
                return this->m_parent != nullptr;
            }

            /**
             * Access the locked data.
             *
             * This method should only be used if the LockedPtr is valid.
             */
            CDataType* operator->() const
            {
                return &m_parent->m_data;
            }

            /**
             * Access the locked data.
             *
             * This method should only be used if the LockedPtr is valid.
             */
            CDataType& operator*() const
            {
                return m_parent->m_data;
            }

        private:
            explicit LockedPtr(CSynchronizedType* parent)
                : /*m_lock{parent->m_mutex}*/
                , m_parent{parent}
            {}

            template<class Rep, class Period>
            LockedPtr(CSynchronizedType* parent,
                      const std::chrono::duration<Rep, Period>& duration)
                : m_lock{parent->m_mutex, duration}
                , m_parent{parent}
            {
                if(!m_lock)
                {
                    m_parent = nullptr;
                }
            }

            LockedPtr(LockedPtr&& rhs) noexcept
                : m_lock{std::move(rhs.m_lock)}
                , m_parent{exchange(rhs.m_parent, nullptr)}
            {}
            LockedPtr& operator=(LockedPtr&& rhs) noexcept
            {
                m_lock   = std::move(rhs.m_lock);
                m_parent = exchange(rhs.m_parent, nullptr);
                return *this;
            }

        private:
            LockType m_lock;
            CSynchronizedType* m_parent = nullptr;
        };

    }  // namespace details

    /* ---------------------------------------------------------------------------------------*/
    /*!
        Synchronization wrapper that locks the underlying data with a mutex when accesing it.
        This is a simplified version of folly::Synchronized.

        @param TSharedLock  std::shared_lock<std::shared_timed_mutex>, 
                            when TMutex = std::shared_timed_mutex.

        @date Fri Aug 24 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TData,
             typename TMutex         = std::recursive_mutex,
             typename TExclusiveLock = std::unique_lock<TMutex>,
             typename TSharedLock    = std::unique_lock<TMutex>>
    class Synchronized
    {
    public:
        using Data          = TData;
        using Mutex         = TMutex;
        using ExclusiveLock = TExclusiveLock;
        using SharedLock    = TSharedLock;

    public:
        using ConstLockedPtr = details::LockedPtr<TSharedLock, const Synchronized>;
        using LockedPtr      = details::LockedPtr<ExclusiveLock, Synchronized>;

    private:
        friend ConstLockedPtr;
        friend LockedPtr;

    public:
        template<typename... Args>
        explicit Synchronized(Args&&... args) noexcept(noexcept(Data(std::forward<Args>(args)...)))
            : m_data(std::forward<Args>(args)...)
        {}

        /**
         * Acquire an exclusive lock, and return a LockedPtr that can be used to
         * safely access the datum.
         *
         * LockedPtr offers operator -> and * to provide access to the datum.
         * The lock will be released when the LockedPtr is destroyed.
         */
        LockedPtr wlock()
        {
            return LockedPtr{this};
        }

        /**
         * Attempts to acquire the lock, or fails if the timeout elapses first.
         * If acquisition is unsuccessful, the returned LockedPtr will be null.
         *
         * (Use LockedPtr::operator bool() or LockedPtr::isNull() to check for
         * validity.)
         */
        template<class Rep, class Period>
        LockedPtr wlock(const std::chrono::duration<Rep, Period>& timeout)
        {
            return LockedPtr(this, timeout);
        }

        /**
         * Acquire a read lock, and return a ConstLockedPtr that can be used to
         * safely access the datum.
         */
        ConstLockedPtr rlock() const
        {
            return ConstLockedPtr(this);
        }

        /**
         * Attempts to acquire the lock, or fails if the timeout elapses first.
         * If acquisition is unsuccessful, the returned LockedPtr will be null.
         *
         * (Use LockedPtr::operator bool() or LockedPtr::isNull() to check for
         * validity.)
         */
        template<class Rep, class Period>
        ConstLockedPtr rlock(const std::chrono::duration<Rep, Period>& timeout) const
        {
            return ConstLockedPtr(this, timeout);
        }

        /**
         * Invoke a function while holding the lock exclusively.
         *
         * A reference to the datum will be passed into the function as its only
         * argument.
         *
         * This can be used with a lambda argument for easily defining small critical
         * sections in the code.  For example:
         *
         *   auto value = obj.withWLock([](auto& data) {
         *     data.doStuff();
         *     return data.getValue();
         *   });
         */
        template<class Function>
        auto withWLock(Function&& function)
        {
            return function(*wlock());
        }

        /**
         * Invoke a function while holding an the lock in shared mode.
         *
         * A const reference to the datum will be passed into the function as its
         * only argument.
         */
        template<class Function>
        auto withRLock(Function&& function) const
        {
            return function(*rlock());
        }

    private:
        mutable Mutex m_mutex;  //!< Mutex for `m_data`.
        Data m_data;            //!< The actual guarded data.
    };
};  // namespace executionGraph

#endif