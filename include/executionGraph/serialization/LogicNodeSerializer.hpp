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
            `TNodeSerialize` needs to fullfill the requirements of `StaticFactory`.

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
                NodeId id        = logicNode.id();
                std::string type = logicNode.type()->str();

                // Dispatch to the correct serialization read function
                // the factory reads and returns the logic node
                auto optNode = FactoryRead::create(rttr::type::get_by_name(type), id, logicNode);
                if(optNode)
                {
                    return std::move(*optNode);
                }
                return nullptr;
            }

            //! Store a logic node by using the builder `builder`.
            static flatbuffers::Offset<serialization::LogicNode>
            write(serialization::LogicNodeBuilder& builder, const NodeBaseType& node)
            {
                NodeId id        = node.getId();
                std::string type = rttr::type::get(node).get_name();

                // // Dispatch to the correct serialization write function
                // // the factory writes the additional data of the flexbuffer
                using DataOffset                  = flatbuffers::Offset<flatbuffers::Vector<uint8_t>>;
                std::optional<DataOffset> optData = FactoryWrite::create(rttr::type::get_by_name(type), builder, node);

                // // Build the logic node
                // auto typeOffsets = builder.CreateString(type);
                // LogicNodeBuilder lnBuilder(builder);
                // lnBuilder.add_id(id);
                // lnBuilder.add_type(typeOffsets);

                // // Add the additional flexbuffer data
                // if(optData)
                // {
                //     lnBuilder.add_data(*optData);
                // }

                // return lnBuilder.Finish()
            }

        private:
            using FactoryWrite = StaticFactory<CreatorListWrite>;
            using FactoryRead  = StaticFactory<CreatorListRead>;
        };
    }  // namespace serialization
}  // namespace executionGraph
#endif