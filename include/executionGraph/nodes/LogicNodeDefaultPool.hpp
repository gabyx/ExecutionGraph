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

#ifndef executionGraph_nodes_LogicNodeDefaultPool_hpp
#define executionGraph_nodes_LogicNodeDefaultPool_hpp

#include <type_traits>
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/nodes/LogicNode.hpp"
#include "executionGraph/nodes/LogicSocket.hpp"

namespace executionGraph
{
    //! Provides a pool of default output sockets for the configuration `TConfig`.
    template<typename TConfig>
    class LogicNodeDefaultPool final : public TConfig::NodeBaseType
    {
    public:
        EXECGRAPH_TYPEDEF_CONFIG(TConfig);
        using Base = typename TConfig::NodeBaseType;

        template<typename... Args>
        LogicNodeDefaultPool(Args&&... args)
            : TConfig::NodeBaseType(std::forward<Args>(args)...)
        {
            // Add a ouput socket with a default-initialized value.
            auto add = [&](auto&& type) {
                using DataType = std::remove_cv_t<std::remove_reference_t<decltype(type)>>;
                this->template addOSock<DataType>(DataType{});
            };
            // Add output socket with default values for all types!
            meta::for_each(SocketTypes{}, add);
        }

        //! Connects the input socket `inSocket` to this default output socket which is
        //! specified by `inSocket.getDefaultOutputSocketIndex()` which cooresponds
        //! by default to the type-matching output socket id of this class.
        void connectIfDangling(SocketInputBaseType& inSocket)
        {
            if(inSocket.getConnectionCount() == 0)
            {
                IndexType defaultOutSocketID = inSocket.getDefaultOutputSocketIndex();
                if(this->hasOSocket(defaultOutSocketID))
                {
                    inSocket.setGetLink(this->getOSocket(defaultOutSocketID));
                }
                else
                {
                    EXECGRAPH_THROW_EXCEPTION_TYPE("Default output socket id: " << defaultOutSocketID
                                                                                << "does not exist in default output socket pool!",
                                                   NodeConnectionException);
                }
            }
        }

        //! Sets the global default value with type `T` which needs to be in the list `SocketTypes`.
        template<typename T>
        void setDefaultValue(T&& defaultValue)
        {
            using DataType = std::remove_cv_t<std::remove_reference_t<T>>;
            static_assert(!std::is_same<meta::find<SocketTypes, DataType>, meta::list<>>::value,
                          "Data type T is not in SocketTypes!");
            // Set the global default value
            this->template getOutVal<DataType>(meta::find_index<SocketTypes, DataType>::value) = std::forward<T>(defaultValue);
        }

        //! Add a new default value with type `T` (needs to be in the list `SocketTypes`)
        //! @return the socket index of the new output socket.
        template<typename T>
        IndexType addNewDefaultValue(T&& defaultValue)
        {
            EXECGRAPH_THROW_EXCEPTION("Needs implementation!");
        }

        void reset() {}
        void compute() {}
    };
};  // namespace executionGraph

#endif
