// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue Aug 28 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_common_Deferred_hpp
#define executionGraph_common_Deferred_hpp

#include <functional>

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
        A RAII-class which deferes the execution of a user-defined function to the end of the 
        lifetime of such an instance, e.g. calling the function in the destructor. 

        @date Tue Aug 28 2018
        @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    class Deferred final
    {
        template<typename T>
        Deferred(T&& function)
            : m_defered(function)
        {}

        ~Deferred() { m_defered(); }

        //! Copy/Assignment is disabled!
        Deferred(const Deferred&) = delete;
        Deferred& oeprator = (const Deferred&) = delete;

        //! Move/move assignment is allowed
        Deferred(Deferred&&) = default;
        Deferred& operator=(Deferred&&) = default;

    private:
        std::function<void()> m_defered;
    };

    //! Make a Deferred instance.
    template<typename T>
    Deferred makeDeferred(T&& function) { return {function}; }

}  // namespace executionGraph

#endif