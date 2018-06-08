// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Tue May 01 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serializer_LogicNodeSerializer_hpp
#define executionGraph_serializer_LogicNodeSerializer_hpp

#include <meta/meta.hpp>
#include "executionGraph/common/Factory.hpp"
#include "executionGraph/serialization/schemas/LogicNode_generated.h"

namespace executionGraph
{
    namespace serialization
    {
        /* ---------------------------------------------------------------------------------------*/
        /*!
            Serializer which loads a Logic Node.
            `CreatorListWrite` and `CreatorListRead` needs to fullfill 
            the requirements of the template parameter of `executionGrpah::StaticFactory`:

            The CreatorListRead contains Creators with the following interface:
            @code
                static std::unique_ptr<NodeBaseType>
                create(const serialization::LogicNode& node)
            @endcode

            The CreatorListWrite contains Creators with the following interface:
            @code
                static std::pair<const uint8_t*, std::size_t>
                create(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
            @endcode

            @date Tue May 01 2018
            @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
        */
        /* ---------------------------------------------------------------------------------------*/
        template<typename TConfig,
                 typename CreatorListWrite,
                 typename CreatorListRead>
        class LogicNodeSerializer final
        {
        public:
            EXECGRAPH_TYPEDEF_CONFIG(TConfig);

        public:
            LogicNodeSerializer()  = default;
            ~LogicNodeSerializer() = default;

        public:
            //! Load a logic node from a `flatbuffers::Offset`.
            static std::unique_ptr<NodeBaseType>
            read(const serialization::LogicNode& logicNode)
            {
                std::string type = logicNode.type()->str();
                // Dispatch to the correct serialization read function
                // the factory reads and returns the logic node
                auto rttrType = rttr::type::get_by_name(type);
                auto optNode  = FactoryRead::create(rttrType, logicNode);
                if(optNode)
                {
                    return std::move(*optNode);
                }
                else
                {
                    // try to construct over RTTR
                    EXECGRAPH_THROW_EXCEPTION_IF(!rttrType.is_derived_from(rttr::type::get<NodeBaseType>()), "Type: " << type << "is not derived from NodeBaseType!"
                                                                                                                                 "Did you correctly init RTTR?");

                    rttr::variant instance;
                    if(logicNode.name())
                    {
                        rttr::constructor ctor = rttrType.get_constructor({rttr::type::get<NodeId>(),
                                                                           rttr::type::get<const std::string&>()});
                        EXECGRAPH_THROW_EXCEPTION_IF(!ctor.is_valid(), "Ctor is invalid for type: " << type);
                        instance = ctor.invoke(logicNode.id(), logicNode.name()->str());
                    }
                    else
                    {
                        rttr::constructor ctor = rttrType.get_constructor({rttr::type::get<NodeId>()});
                        EXECGRAPH_THROW_EXCEPTION_IF(!ctor.is_valid(), "Ctor is invalid for type: " << type);
                        instance = ctor.invoke(logicNode.id());
                    }
                    EXECGRAPH_LOG_DEBUG(instance.get_type().get_name().to_string());
                    EXECGRAPH_THROW_EXCEPTION_IF(!instance.is_valid(), "Instance is not valid!");
                    EXECGRAPH_THROW_EXCEPTION_IF(!instance.get_type().is_pointer(), "Instance type needs to be a pointer!");

                    return std::unique_ptr<NodeBaseType>{instance.get_value<NodeBaseType*>()};  // Return the instance
                }
            }

            //! Store a logic node by using the builder `builder`.
            static flatbuffers::Offset<serialization::LogicNode>
            write(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
            {
                NodeId id       = node.getId();
                auto nameOffset = builder.CreateString(node.getName());

                std::string type = rttr::type::get(node).get_name().to_string();
                auto typeOffset  = builder.CreateString(type);

                // Dispatch to the correct serialization write function.
                // The factory writes the additional data of the flexbuffer `data` field
                flatbuffers::FlatBufferBuilder builderData;
                flatbuffers::Offset<flatbuffers::Vector<uint8_t>> dataOffset;

                std::optional<std::pair<const uint8_t*, std::size_t>>
                    optData = FactoryWrite::create(rttr::type::get(node),
                                                   builderData,
                                                   node);

                if(optData)
                {
                    dataOffset = builder.CreateVector(optData->first, optData->second);
                }

                // Build the logic node
                LogicNodeBuilder lnBuilder(builder);
                lnBuilder.add_id(id);
                lnBuilder.add_type(typeOffset);
                lnBuilder.add_name(nameOffset);
                if(optData)
                {
                    lnBuilder.add_data(dataOffset);
                }
                return lnBuilder.Finish();
            }

        private:
            using FactoryWrite = StaticFactory<CreatorListWrite>;
            using FactoryRead  = StaticFactory<CreatorListRead>;
        };
    }  // namespace serialization
}  // namespace executionGraph
#endif