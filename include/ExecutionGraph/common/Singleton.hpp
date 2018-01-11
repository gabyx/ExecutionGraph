//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Tue Jan 09 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef ExecutionGraph_common_Singelton_hpp
#define ExecutionGraph_common_Singelton_hpp

#include <memory>
#include "ExecutionGraph/common/Asserts.hpp"

namespace executionGraph
{
    //! A singelton class which does not construct the type on the fly like most implementations
    //! The Singelton needs to be explicity instanciated somewhere, best in the main(...) function!
    template<typename T>
    class Singleton
    {
    private:
        //! Explicit private copy constructor. This is a forbidden operation.
        Singleton(const Singleton<T>&);

        //! Private operator= . This is a forbidden operation.
        Singleton& operator=(const Singleton<T>&);

    private:
        static T* m_instance;  //!< The single instance of type T.

    public:
        Singleton(void)
        {
            EXECGRAPH_ASSERT(!m_instance, "m_instance != nullptr : " << typeid(*m_instance).name());
            m_instance = static_cast<T*>(this);
        }
        ~Singleton(void)
        {
            m_instance = nullptr;
        }

        static T& getInstance(void)
        {
            EXECGRAPH_ASSERT(m_instance, "m_instance != nullptr : " << typeid(*m_instance).name());
            return (*m_instance);
        }
    };

    template<typename T>
    T* Singleton<T>::m_instance = nullptr;

//! Instanciate a singelton `name` with type `type` and ctor arguments `ctor_args`.
#define EXECGRAPH_INSTANCIATE_SINGLETON_CTOR(type, name, ctor_args) \
    auto name = std::unique_ptr<type>(new type ctor_args);

//! Instanciate a singelton `name` with type `type`.
#define EXECGRAPH_INSTANCIATE_SINGLETON(type, name) INSTANCIATE_UNIQUE_SINGELTON_CTOR(type, name, ())
}

#endif
