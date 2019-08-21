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

#pragma once

#include <memory>
#include <vector>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/DemangleTypes.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"

namespace executionGraph
{
    class LogicSocketInputBase;
    class LogicSocketOutputBase;

    /**
 @brief The execution node (node) class which is the base class
        for every node in an execution graph.

        // clang-format off
        General Concept
                               +-------------+                                       +-------------+
                               | LogicNode A |                                       | LogicNode B |
                               |             |                                       |             |
                               |     +-------+-----+                           +-----+-----+       |
                               |     |  Out Socket |                           | In Socket |       |
                               |     |             |                           |           |       |
                               |     | m_writeTo[0]|@------( Write-Link )----> |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |             | <-----(  Get-Link  )-----@| m_getFrom |       |
                               |     |             |                           |           |       |
                               |     |             |                           |           |       |
                               |     |   T m_data  |                           | T* m_data |       |
                               |     +-------+-----+                           +-----+-----+       |
                               |             |                                       |             |
                               +-------------+                                       +-------------+

                Function Behavior in Out Socket:  (this =  Out)  ++      Function Behavior in In Socket:  (this = In)
                ================================                 ||      ===============================
                                                                 ||
                -data():  gets this->m_data,                 ||      -data(): gets the value *this->m_data
                                                                 ||                   Exception if nullptr
                                                                 ||
                                                                 ||
                -setData():  set all Write-Links directly       ||      -setData(): sets internal this->m_data
                              (m_writeTo array) and set          ||
                              this->m_data also because this     ||
                              might have other                   ||
                              Get-Links                          ||
                                                                 ++
        // clang-format on
*/

    class LogicNode
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

        using SocketInputListType  = std::vector<SocketPointer<LogicSocketInputBase>>;
        using SocketOutputListType = std::vector<SocketPointer<LogicSocketOutputBase>>;

    public:
        //! The basic constructor of a node.
        LogicNode(NodeId id = nodeIdInvalid)
            : m_id(id)
        {
        }

        LogicNode(const LogicNode&) = default;
        LogicNode(LogicNode&&)      = default;

        virtual ~LogicNode() = default;

        //! The reset function.
        virtual void reset() = 0;

        //! The main compute function of this execution node.
        virtual void compute() = 0;

        inline NodeId getId() const { return m_id; }
        inline void setId(NodeId id) { m_id = id; }

        //! Get the list of input sockets.
        inline const SocketInputListType& getInputs() const { return m_inputs; }
        inline SocketInputListType& getInputs() { return m_inputs; }
        //! Get the list of output sockets.
        inline const SocketOutputListType& getOutputs() const { return m_outputs; }
        inline SocketOutputListType& getOutputs() { return m_outputs; }

        IndexType connectedInputCount() const;
        IndexType connectedOutputCount() const;

        // bool hasISocket(SocketIndex idx) const { return idx < m_inputs.size(); }
        // bool hasOSocket(SocketIndex idx) const { return idx < m_outputs.size(); }

        // //! Check if input socket at index `idx` has type `T`.
        // template<typename T>
        // bool hasISocketType(SocketIndex idx) const
        // {
        //     return hasISocket(idx) && getISocket(idx).template isType<T>();
        // }

        // //! Check if output socket at index `idx` has type `T`.
        // template<typename T>
        // bool hasOSocketType(SocketIndex idx) const
        // {
        //     return hasOSocket(idx) && getOSocket(idx).template isType<T>();
        // }

        // //! Get the input socket at index `idx`.
        // LogicSocketInputBase& getISocket(SocketIndex idx)
        // {
        //     EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
        //     return *m_inputs[idx];
        // }
        // const LogicSocketInputBase& getISocket(SocketIndex idx) const
        // {
        //     return const_cast<LogicNode*>(this)->getISocket(idx);
        // }
        // //! Get the output socket at index `idx`.
        // LogicSocketOutputBase& getOSocket(SocketIndex idx)
        // {
        //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
        //     return *m_outputs[idx];
        // }
        // const LogicSocketOutputBase& getOSocket(SocketIndex idx) const
        // {
        //     return const_cast<LogicNode*>(this)->getOSocket(idx);
        // }

        // //! Get the input socket of type `T` at index `idx`.
        // template<typename T>
        // auto& getISocket(SocketIndex idx);
        // template<typename T>
        // const auto& getISocket(SocketIndex idx) const;

        // //! Get the output socket of type `T` at index `idx`.
        // template<typename T>
        // auto& getOSocket(SocketIndex idx);
        // template<typename T>
        // const auto& getOSocket(SocketIndex idx) const;

        // //! Get input socket value `T` at index `idx`.
        // template<typename T>
        // const T& getInVal(SocketIndex idx) const;

        // //! Get the read/write output socket value `T` of the output socket at index `idx`.
        // template<typename T>
        // T& getOutVal(SocketIndex idx);
        // template<typename T>
        // const T& getOutVal(SocketIndex idx) const;

        // //! Get the output socket value from a SocketDeclaration `OutputSocketDeclaration`.
        // template<typename TSocketDeclaration,
        //          EXECGRAPH_SFINAE_ENABLE_IF((meta::is<TSocketDeclaration, details::OutputSocketDeclaration>::value))>
        // typename TSocketDeclaration::Data& data();
        // //! Get the output socket value from a SocketDeclaration `OutputSocketDeclaration`.
        // template<typename TSocketDeclaration,
        //          EXECGRAPH_SFINAE_ENABLE_IF((meta::is<TSocketDeclaration, details::OutputSocketDeclaration>::value))>
        // const typename TSocketDeclaration::Data& data() const;

        // //! Get the input socket value from a SocketDeclaration `InputSocketDeclaration`.
        // template<typename TSocketDeclaration,
        //          EXECGRAPH_SFINAE_ENABLE_IF((meta::is<TSocketDeclaration, details::InputSocketDeclaration>::value))>
        // const typename TSocketDeclaration::Data& data() const;

        // //! Constructs a Get-Link to get the data from output socket at index `outS`
        // //! of node `outN` at the input socket at index `inS` of node
        // //! `inN`.
        // void setGetLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
        // {
        //     EXECGRAPH_THROW_TYPE_IF(!outN.hasOSocket(outS) || !inN.hasISocket(inS),
        //                             NodeConnectionException,
        //                             "Wrong socket indices:  outNode: '{0}' outSocketIdx: '{1}' "
        //                             "inNode: '{2}' inSocketIdx: '{3}'",
        //                             outN.getId(),
        //                             outS,
        //                             inN.getId(),
        //                             inS);

        //     inN.getISocket(inS).setGetLink(outN.getOSocket(outS));
        // }

        // //! Constructs a Get-Link to get the data from output socket at index `outS`
        // //! of node `outN` at the input socket at index `inS` of this node.
        // inline void setGetLink(LogicNode& outN, SocketIndex outS, SocketIndex inS)
        // {
        //     setGetLink(outN, outS, *this, inS);
        // }

        // //! Remove an existing Get-Link from input socket at index `inS` of
        // //! node `node`.
        // void removeGetLink(LogicNode& node, SocketIndex inS)
        // {
        //     EXECGRAPH_THROW_TYPE_IF(!node.hasISocket(inS),
        //                             NodeConnectionException,
        //                             "Wrong socket indices:"
        //                             "node: '{0}' inSocketIdx: '{1}' (nIns: '{2}')",
        //                             node.getId(),
        //                             inS,
        //                             node.getInputs().size());

        //     node.getISocket(inS).removeGetLink();
        // }
        // //! Remove an existing Get-Link from input socket at index `inS`.
        // inline void removeGetLink(SocketIndex inS)
        // {
        //     removeGetLink(*this, inS);
        // }

        // //! Constructs a Write-Link to write the data of output socket at index
        // //! `outS` of node `outN` to the input socket at index `inS` of node `inN`.
        // static void addWriteLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
        // {
        //     EXECGRAPH_THROW_TYPE_IF(!outN.hasOSocket(outS) || !inN.hasISocket(inS),
        //                             NodeConnectionException,
        //                             "Wrong socket indices:  outNode: '{0}' outSocketIdx: '{1}' "
        //                             "inNode: '{2}' inSocketIdx: '{3}' (nOuts: '{4}', nIns: '{5}' )",
        //                             outN.getId(),
        //                             outS,
        //                             inN.getId(),
        //                             inS,
        //                             outN.getOutputs().size(),
        //                             inN.getInputs().size());

        //     outN.getOSocket(outS).addWriteLink(inN.getISocket(inS));
        // }
        // //! Constructs a Write-Link to write the data of output socket at index
        // //! `outS`of this node to the input socket at index `inS`.
        // inline void addWriteLink(SocketIndex outS, LogicNode& inN, SocketIndex inS)
        // {
        //     addWriteLink(*this, outS, inN, inS);
        // }

        // //! Remove a Write-Link to write the data of output socket at index
        // //! `outS` of node `outN` to the input socket at index `inS` of node `inN`.
        // static void removeWriteLink(LogicNode& outN, SocketIndex outS, LogicNode& inN, SocketIndex inS)
        // {
        //     EXECGRAPH_THROW_TYPE_IF(!outN.hasOSocket(outS) || !inN.hasISocket(inS),
        //                             NodeConnectionException,
        //                             "Wrong socket indices:  outNode: '{0}' outSocketIdx: '{1}' "
        //                             "inNode: '{2}' inSocketIdx: '{3}' (nOuts: '{4}', nIns: '{5}' )",
        //                             outN.getId(),
        //                             outS,
        //                             inN.getId(),
        //                             inS,
        //                             outN.getOutputs().size(),
        //                             inN.getInputs().size());

        //     outN.getOSocket(outS).removeWriteLink(inN.getISocket(inS));
        // }

        // //! Remove a Write-Link to write the data of output socket at index
        // //! `outS` of this node to the input socket at index `inS` of node `inN`.
        // inline void removeWriteLink(SocketIndex outS, LogicNode& inN, SocketIndex inS)
        // {
        //     removeWriteLink(*this, outS, inN, inS);
        // }

        //! Adding of sockets is protected and should only be done in constructor!
        //@{
    protected:
        // //! Add an input socket with default value from the default output socket `defaultOutputSocketId`.
        // template<typename TData>
        // void addISock()
        // {
        //     SocketIndex id = m_inputs.size();

        //     auto p = SocketPointer<LogicSocketInputBase>(
        //         new LogicSocketInput<TData>(id, *this),
        //         [](LogicSocketInputBase* s) { delete static_cast<LogicSocketInput<TData>*>(s); });
        //     m_inputs.push_back(std::move(p));
        // }

        // //! Add an output socket with default value `defaultValue`.
        // template<typename TData, typename T>
        // void addOSock(T&& defaultValue)
        // {
        //     SocketIndex id = m_outputs.size();

        //     auto p = SocketPointer<LogicSocketOutputBase>(
        //         new LogicSocketOutput<TData>(std::forward<T>(defaultValue), id, *this),
        //         [](LogicSocketOutputBase* s) { delete static_cast<LogicSocketOutput<TData>*>(s); });
        //     m_outputs.push_back(std::move(p));
        // }

        // //! Add all input sockets defined in the type list `SocketDeclList`.
        // template<typename SocketDeclList,
        //          EXECGRAPH_SFINAE_ENABLE_IF((meta::is<SocketDeclList, details::InputSocketDeclarationList>::value))>
        // void addSockets()
        // {
        //     auto add = [&](auto socketDeclaration) {
        //         using SocketDeclaration = decltype(socketDeclaration);
        //         this->template addISock<typename SocketDeclaration::Data>();
        //     };

        //     meta::for_each(typename SocketDeclList::TypeList{}, add);
        // }

        // //! Add all output sockets defined in the type list `SocketDeclList` where each socket has
        // //! the corresponding default value in `defaultValues`.
        // template<typename SocketDeclList,
        //          typename... Args,
        //          EXECGRAPH_SFINAE_ENABLE_IF((meta::is<SocketDeclList, details::OutputSocketDeclarationList>::value))>
        // void addSockets(std::tuple<Args...>&& defaultValues)
        // {
        //     auto add = [&](auto socketDeclaration) {
        //         using SocketDeclaration = decltype(socketDeclaration);
        //         this->template addOSock<typename SocketDeclaration::Data>(std::move(std::get<SocketDeclaration::Index::value>(defaultValues)));
        //     };

        //     meta::for_each(typename SocketDeclList::TypeList{}, add);
        // }
        //@}

    protected:
        NodeId m_id;                     //!< The id of the node.
        SocketInputListType m_inputs;    //!< The input sockets.
        SocketOutputListType m_outputs;  //!< The output sockets.
    };

    // template<typename TConfig>
    // template<typename T>
    // auto& LogicNode<TConfig>::getISocket(SocketIndex idx)
    // {
    //     EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    //     return m_inputs[idx]->template castToType<T>();
    // }

    // template<typename TConfig>
    // template<typename T>
    // const auto& LogicNode<TConfig>::getISocket(SocketIndex idx) const
    // {
    //     EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    //     return m_inputs[idx]->template castToType<T>();
    // }

    // template<typename TConfig>
    // template<typename T>
    // auto& LogicNode<TConfig>::getOSocket(SocketIndex idx)
    // {
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<T>();
    // }

    // template<typename TConfig>
    // template<typename T>
    // const auto& LogicNode<TConfig>::getOSocket(SocketIndex idx) const
    // {
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<T>();
    // }

    // template<typename TConfig>
    // template<typename T>
    // const T& LogicNode<TConfig>::getInVal(SocketIndex idx) const
    // {
    //     EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    //     return m_inputs[idx]->template castToType<T>().data();
    // }

    // template<typename TConfig>
    // template<typename T>
    // T& LogicNode<TConfig>::getOutVal(SocketIndex idx)
    // {
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<T>().data();
    // }

    // template<typename TConfig>
    // template<typename T>
    // const T& LogicNode<TConfig>::getOutVal(SocketIndex idx) const
    // {
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<T>().data();
    // }

    // template<typename TConfig>
    // template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF_IMPL((meta::is<TSocketDeclaration, details::OutputSocketDeclaration>::value))>
    // typename TSocketDeclaration::Data& LogicNode<TConfig>::data()
    // {
    //     auto idx = TSocketDeclaration::Index::value;
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<typename TSocketDeclaration::Data>().data();
    // }

    // template<typename TConfig>
    // template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF_IMPL((meta::is<TSocketDeclaration, details::InputSocketDeclaration>::value))>
    // const typename TSocketDeclaration::Data& LogicNode<TConfig>::data() const
    // {
    //     auto idx = TSocketDeclaration::Index::value;
    //     EXECGRAPH_ASSERT(idx < m_inputs.size(), "Wrong index!");
    //     return m_inputs[idx]->template castToType<typename TSocketDeclaration::Data>().data();
    // }

    // template<typename TConfig>
    // template<typename TSocketDeclaration, EXECGRAPH_SFINAE_ENABLE_IF_IMPL((meta::is<TSocketDeclaration, details::OutputSocketDeclaration>::value))>
    // const typename TSocketDeclaration::Data& LogicNode<TConfig>::data() const
    // {
    //     auto idx = TSocketDeclaration::Index::value;
    //     EXECGRAPH_ASSERT(idx < m_outputs.size(), "Wrong index!");
    //     return m_outputs[idx]->template castToType<typename TSocketDeclaration::Data>().data();
    // }

}  // namespace executionGraph
