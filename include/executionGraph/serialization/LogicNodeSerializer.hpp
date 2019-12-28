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

#pragma once

#include <type_traits>
#include <flatbuffers/flatbuffers.h>
#include <meta/meta.hpp>
#include <rttr/type>
#include "executionGraph/common/Factory.hpp"
#include "executionGraph/common/MetaVisit.hpp"
#include "executionGraph/serialization/Conversions.hpp"
#include "executionGraph/serialization/SocketTypeDescription.hpp"
#include "executionGraph/serialization/schemas/cpp/LogicNode_generated.h"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
            Serializer which loads a node.
            `NodeSerializerList` contains a type `Writer` and a type `Reader` which 
            both need to fullfill the requirements for `Type` in 
            `executionGrpah::StaticFactory<meta::list<Type,...>>`:

            The `Reader` contains the following interface:
            @code
                static std::unique_ptr<NodeBaseType>
                create(const serialization::LogicNode& node)
            @endcode

            The `Writer` contains Creators with the following interface:
            @code
                static std::pair<const uint8_t*, std::size_t>
                create(flatbuffers::FlatBufferBuilder& builder, const NodeBaseType& node)
            @endcode

            @date Tue May 01 2018
            @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
        */
    /* ---------------------------------------------------------------------------------------*/
    template<typename TConfig,
             typename NodeSerializerList>
    class LogicNodeSerializer final
    {
    public:
        EG_DEFINE_TYPES(TConfig);

    public:
        LogicNodeSerializer()  = default;
        ~LogicNodeSerializer() = default;

    public:
        //! Main load function for a node.
        //! It first tries to construct it by the factory
        //! and uses RTTR construction as a fallback.
        static std::unique_ptr<NodeBaseType>
        read(std::string_view type,
             NodeId nodeId,
             const flatbuffers::Vector<flatbuffers::Offset<serialization::LogicSocket>>* inputSockets  = nullptr,
             const flatbuffers::Vector<flatbuffers::Offset<serialization::LogicSocket>>* outputSockets = nullptr,
             const flatbuffers::Vector<uint8_t>* additionalData                                        = nullptr)
        {
            // Dispatch to the correct serialization read function
            // the factory reads and returns the node
            auto rttrType = rttr::type::get_by_name(rttr::toRttr(type));

            auto optNode = FactoryRead::create(rttrType,
                                               nodeId,
                                               inputSockets,
                                               outputSockets,
                                               additionalData);

            if(optNode)
            {
                EG_THROW_IF(*optNode == nullptr,
                                   "FactoryRead::create provided nullptr for type '{0}'!",
                                   type)
                return std::move(*optNode);
            }
            else
            {
                EG_THROW_IF(additionalData != nullptr,
                                   "Cannot construct type: '{0}' without ReadFactory (add data!)!",
                                   type)
                // try to construct over RTTR
                EG_THROW_IF(!rttrType.is_derived_from(rttr::type::get<NodeBaseType>()),
                                   "Type: '{0}' is not derived from NodeBaseType!"
                                   "Did you correctly init RTTR?",
                                   type);

                rttr::variant instance;

                rttr::constructor ctor = rttrType.get_constructor({rttr::type::get<NodeId>()});
                EG_THROW_IF(!ctor.is_valid(), "Ctor is invalid for type: '{0}'", type);
                instance = ctor.invoke(nodeId);

                EG_THROW_IF(!instance.is_valid(), "Variant instance is not valid!");
                EG_THROW_IF(!instance.get_type().is_pointer(), "Variant instance type needs to be a pointer!");

                return std::unique_ptr<NodeBaseType>{instance.get_value<NodeBaseType*>()};  // Return the instance
            }
        }

        //! Load a node from a `serialization::LogicNode`.
        static std::unique_ptr<NodeBaseType>
        read(const serialization::LogicNode& logicNode)
        {
            return LogicNodeSerializer::read(logicNode.type()->str(),
                                             logicNode.id(),
                                             logicNode.inputSockets(),
                                             logicNode.outputSockets(),
                                             logicNode.data());
        }

        //! Store a node by using the builder `builder`.
        static flatbuffers::Offset<serialization::LogicNode>
        write(flatbuffers::FlatBufferBuilder& builder,
              const NodeBaseType& node,
              bool serializeAdditionalData     = true,
              bool serializeFullSocketTypeSpec = false)
        {
            namespace s = serialization;
            using namespace s;
            namespace fb = flatbuffers;

            NodeId id        = node.getId();
            std::string type = rttr::type::get(node).get_name().to_string();
            auto typeOffset  = builder.CreateString(type);

            // Build all input/output sockets
            auto pairOffs   = writeSockets(builder, node, serializeFullSocketTypeSpec);
            auto inputsOff  = std::get<0>(pairOffs);
            auto outputsOff = std::get<1>(pairOffs);

            // Dispatch to the correct serialization write function.
            // The factory writes the additional data of the flexbuffer `data` field
            fb::FlatBufferBuilder builderData;
            fb::Offset<flatbuffers::Vector<uint8_t>> dataOffset;

            // Write the data (optional because the factory might not have a writer)
            if(serializeAdditionalData)
            {
                std::optional<std::pair<const uint8_t*, std::size_t>>
                    optData = FactoryWrite::create(rttr::type::get(node),
                                                   builderData,
                                                   node);
                if(optData && optData->second != 0)
                {
                    dataOffset = builder.CreateVector(optData->first, optData->second);
                }
            }

            // Build the node
            LogicNodeBuilder lnBuilder(builder);
            lnBuilder.add_id(id);
            lnBuilder.add_type(typeOffset);
            lnBuilder.add_inputSockets(inputsOff);
            lnBuilder.add_outputSockets(outputsOff);
            if(!dataOffset.IsNull())
            {
                lnBuilder.add_data(dataOffset);
            }
            return lnBuilder.Finish();
        }

    private:
        //! Write all input/output sockets.
        static auto writeSockets(flatbuffers::FlatBufferBuilder& builder,
                                 const NodeBaseType& node,
                                 bool fullTypeSpec = false)
        {
            namespace s = serialization;
            using namespace s;

            auto& socketDescriptions = getSocketDescriptions<Config>();

            auto write = [&](const auto& sockets) {
                std::vector<flatbuffers::Offset<LogicSocket>> socketOffs;
                for(auto& socket : sockets)
                {
                    EG_ASSERT(socket->type() < socketDescriptions.size(), "Socket type wrong!");

                    flatbuffers::Offset<flatbuffers::String> typeOff, typeNameOff;
                    if(fullTypeSpec)
                    {
                        typeOff     = builder.CreateString(socketDescriptions[socket->type()].m_type);
                        typeNameOff = builder.CreateString(socketDescriptions[socket->type()].m_name);
                    }

                    LogicSocketBuilder soBuilder(builder);
                    soBuilder.add_typeIndex(socket->type());
                    soBuilder.add_type(typeOff);
                    soBuilder.add_typeName(typeNameOff);

                    soBuilder.add_index(socket->getIndex());

                    socketOffs.emplace_back(soBuilder.Finish());
                }
                return builder.CreateVector(socketOffs);
            };

            return std::make_pair(write(node.getInputs()),
                                  write(node.getOutputs()));
        }

        //! Helper to check the socket with name `name` to the node `node`.
        template<bool checkInput>
        struct SocketChecker
        {
            template<typename T>
            void invoke(NodeBaseType& node, SocketIndex index)
            {
                bool correct = false;
                if(checkInput)
                {
                    correct = node.template hasISocketType<T>(index);
                }
                else
                {
                    correct = node.template hasOSocketType<T>(index);
                }
                EG_THROW_IF(!correct,
                                   "Node '{0}' has no {1}"
                                   "socket type '{2}' at index '{3}'! "
                                   "The deserialization should have added this sockets throught the constructor! ",
                                   ((checkInput) ? "input" : "output"),
                                   node->getId(),
                                   rttr::type::get<T>().get_name().to_string(),
                                   index);
            }
        };

        //! Check all sockets for LogicNode `node`.
        static void
        checkSockets(NodeBaseType& node,
                     const serialization::LogicNode& logicNode)
        {
            namespace s = serialization;
            using namespace s;

            auto read = [&](flatbuffers::Vector<flatbuffers::Offset<LogicSocket>>* sockets,
                            auto&& checker) {
                if(sockets == nullptr)
                {
                    return;
                }
                for(auto socketOff : *sockets)
                {
                    meta::visit<typename Config::SocketTypes>(checker,
                                                              socketOff->type(),
                                                              node,
                                                              socketOff->index());
                }
            };

            read(logicNode.inputSockets(), SocketChecker<true>{});
            read(logicNode.outputSockets(), SocketChecker<false>{});
        }

    private:
        //! Type `T::Writer` detector
        template<typename T, typename = void>
        struct hasWriter : std::false_type
        {};
        template<typename T>
        struct hasWriter<T, std::void_t<typename T::Writer>> : std::true_type
        {};
        //! Type `T::Reader` detector
        template<typename T, typename = void>
        struct hasReader : std::false_type
        {};
        template<typename T>
        struct hasReader<T, std::void_t<typename T::Writer>> : std::true_type
        {};

        template<typename T>
        using writeExtractor = typename T::Writer;
        template<typename T>
        using readExtractor = typename T::Reader;

        using CreatorListWrite = meta::transform<meta::filter<NodeSerializerList, meta::quote<hasWriter>>,
                                                 meta::quote<writeExtractor>>;
        using CreatorListRead  = meta::transform<meta::filter<NodeSerializerList, meta::quote<hasReader>>,
                                                meta::quote<readExtractor>>;
        using FactoryWrite     = StaticFactory<CreatorListWrite>;
        using FactoryRead      = StaticFactory<CreatorListRead>;
    };
}  // namespace executionGraph
