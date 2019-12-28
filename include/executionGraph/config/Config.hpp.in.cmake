//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//! 
//!  @date Mon Jan 08 2018
//!  @author Gabriel Nützi, <gnuetzi (at) gmail (døt) com>
//! 
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#ifndef executionGraph_Config_Config_hpp_in_cmake
#define executionGraph_Config_Config_hpp_in_cmake

//! Settings for the ExecutionGraph Library
namespace executionGraph{

    //! Library Version
    static const char* versionString       = "@ExecutionGraph_VERSION_MAJOR@."
                                             "@ExecutionGraph_VERSION_MINOR@."
                                             "@ExecutionGraph_VERSION_PATCH@";
    static const unsigned int versionMajor =  @ExecutionGraph_VERSION_MAJOR@ ;
    static const unsigned int versionMinor =  @ExecutionGraph_VERSION_MINOR@ ;
    static const unsigned int versionPatch =  @ExecutionGraph_VERSION_PATCH@ ;
    
    //! Force log level
    #define EG_FORCE_MSGLOG_LEVEL @ExecutionGraph_FORCE_MSGLOG_LEVEL@
    
    #cmakedefine ExecutionGraph_OPENMP_SUPPORT
    #ifdef ExecutionGraph_OPENMP_SUPPORT
        static const bool haveOpenMpSupport = true;
        #cmakedefine ExecutionGraph_OPENMP_USE_NTHREADS
        #ifdef ExecutionGraph_OPENMP_USE_NTHREADS
            #define EG_OPENMP_USE_NTHREADS
            static const int openMpUseNThreads = EG_OPENMP_NTHREADS;
            #define EG_OPENMP_NTHREADS @ExecutionGraph_OPENMP_NTHREADS@
            #define EG_OPENMP_NUMTHREADS num_threads(EG_OPENMP_NTHREADS)
        #else
            static const unsigned int openMpUseNThreads = 0;
            #define EG_OPENMP_NUMTHREADS
        #endif
    #else
        static const bool haveOpenMpSupport = false;
        static const unsigned int openMpUseNThreads = 0;
    #endif
    
    #cmakedefine ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS
    #ifdef ExecutionGraph_THROW_IF_BAD_SOCKET_CASTS
        #define EG_THROW_IF_BAD_SOCKET_CASTS
        static const bool throwIfBadSocketCast = true;
    #else
        static const bool throwIfBadSocketCast = false;
    #endif

    #cmakedefine ExecutionGraph_THROW_IF_NODE_DATA_NO_STORAGE
    #ifdef ExecutionGraph_THROW_IF_NODE_DATA_NO_STORAGE
        #define EG_THROW_IF_NODE_DATA_NO_STORAGE
        static const bool throwIfNodeDataNoStorage = true;
    #else
        static const bool throwIfNodeDataNoStorage = false;
    #endif

    #cmakedefine ExecutionGraph_HAS_RTTR
    #ifdef ExecutionGraph_HAS_RTTR
        #define EG_HAS_RTTR
    #else
        #undef EG_HAS_RTTR
    #endif

    #cmakedefine ExecutionGraph_HAS_FLATBUFFERS
    #ifdef ExecutionGraph_HAS_FLATBUFFERS
        #define EG_HAS_FLATBUFFERS
    #else
        #undef EG_HAS_FLATBUFFERS
    #endif

}


#endif
