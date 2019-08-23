// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Aug 21 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <unordered_set>
#include <meta/meta.hpp>
#include <rttr/type>
#include "executionGraph/common/Assert.hpp"
#include "executionGraph/common/TypeDefs.hpp"
#include "executionGraph/nodes/LogicCommon.hpp"
#include "executionGraph/nodes/LogicSocketData.hpp"

namespace executionGraph
{
    template<typename>
    class LogicNodeData;

    class LogicDataNodeBase
    {
        template<typename>
        friend class LogicNodeData;

    public:
        EXECGRAPH_DEFINE_TYPES();

        auto type() { return m_type; }

        template<typename T>
        bool isType() const noexcept { return type() == rttr::type::get<T>(); }

        //! Cast to a logic data node of type `LogicDataNode<T>*`.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto& castToType() const noexcept
        {
            if constexpr(throwIfDataNodeNoStorage)
            {
                EXECGRAPH_LOGTHROW_IF(!isType<T>(),
                                      "Casting node data with id '{0}' and type '{1}' into type"
                                      "'{2}' which is wrong!",
                                      id(),
                                      type(),
                                      rttr::type::get<T>().get_name());
            }

            return static_cast<const LogicDataNode<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T>
        auto& castToType() noexcept
        {
            return const_cast<LogicDataNode<T>&>(
                static_cast<const LogicDataNodeBase*>(this)->castToType<T>());
        }

        NodeDataId getId() const noexcept { return m_id; }
        void setId(NodeDataId id) noexcept { m_id = id; }

    protected:
        bool hasData() const noexcept { return m_data != nullptr; }

        template<typename TData>
        void setData(TData* data) noexcept
        {
            EXECGRAPH_ASSERT(rttr::type::get<TData>() == m_type,
                             "You cannot set a pointer to different underlying data!");
            m_data = data;
        }

        template<typename Data>
        const Data& data() noexcept
        {
            if constexpr(throwIfDataNodeNoStorage)
            {
                EXECGRAPH_LOGTHROW_IF(!hasData(),
                                      "Node data with id: {0} has no assigned storage!",
                                      id());
            }

            return *static_cast<const Data*>(m_data);
        }
        template<typename Data>
        Data& data() noexcept
        {
            return const_cast<Data&>(static_cast<const Base&>(*this).data());
        }

    protected:
        LogicDataNodeBase(rttr::type type,
                          NodeDataId id)
            : m_type(type)
            , m_id(id)
        {}

    private:
        const rttr::type m_type;              //!< The type of this node node.
        NodeDataId m_id = nodeDataIdInvalid;  //!< Id of this node.
        void* m_data    = nullptr;            //!< Fast access pointer to underlying data.
    };

    template<typename TData>
    class DataStorage final
    {
    public:
        using Data = TData;

    public:
        LogicSocketData() noexcept = default;

        template<typename T>
        LogicSocketData(T&& value) noexcept
            : m_data(std::forward<T>(value))
        {}

        Data& data() noexcept { return m_data; }
        const Data& data() const noexcept { return m_data; }

    private:
        Data m_data;  //!< The data.
    };

    template<typename TData>
    class LogicDataNodeLinks
    {
    public:
        using InputSocket  = LogicSocketInput<Data>;
        using OutputSocket = LogicSocketOutput<Data>;

    protected:
        DataNodeLink() noexcept = default;

    public:
        ~DataNodeLink() noexcept
        {
            for(auto* output : m_outputs)
            {
                output->onRemoveWriteLink(*this);
            }
            for(auto* input : m_inputs)
            {
                input->onRemoveGetLink(*this);
            }
        }

    private:
        bool onAddWriteLink(const OutputSocket& output) noexcept
        {
            return m_outputs.emplace(&const_cast<OutputSocket&>(output)).second;
        }
        bool onRemoveWriteLink(const OutputSocket& output) noexcept
        {
            return m_outputs.erase(&const_cast<OutputSocket&>(output)).second;
        }

        bool onAddGetLink(const InputSocket& input) noexcept
        {
            return m_inputs.emplace(&const_cast<InputSocket&>(input)).second;
        }
        bool onRemoveGetLink(const InputSocket& input) noexcept
        {
            return m_inputs.erase(&const_cast<InputSocket&>(input)).second;
        }

    public:
        const auto& outputs() const noexcept { return m_outputs; }
        const auto& inputs() const noexcept { return m_inputs; }

    private:
        std::unordered_set<InputSocket*> m_inputs;    //! All inputs getting this data node.
        std::unordered_set<OutputSocket*> m_outputs;  //! All outputs writting to his data node.
    };

    template<typename...>
    class LogicDataNodeRef;

    template<typename TData,
             typename TStorage = void>
    class LogicDataNode final : public LogicDataNodeBase,
                                public LogicDataNodeLinks<TData>
    {
    public:
        using Base = LogicDataNodeBase;
        using Data = TData;
        static_assert(!std::is_const_v<Data>, "Only non-const type allowed!");

        using Storage = meta::if_<std::is_same_v<TStorage, void>,
                                  meta::lazy::invoke<TStorage, Data>,
                                  DataStorage<Data>>;

        using Reference = LogicDataNodeRef<Data, Storage>;

        friend class Reference;
        friend class InputSocket;
        friend class OutputSocket;

    public:
        template<typename... Args>
        LogicDataNode(Args&&... args) noexcept
            : Base(std::forward<Args>(args)...)
        {
        }

        ~LogicDataNode() noexcept
        {
            for(auto* ref : m_refs)
            {
                removeReference(*ref);
            }
        };

        const Data& data() const noexcept
        {
            return data<Data>();
        }
        Data& data() noexcept
        {
            return data<Data>();
        }

    public:
        template<typename... Args>
        void emplaceStorage(Args&&... args) noexcept
        {
            m_storage.emplace(std::forward<Args>(args)...);
            m_data = &m_storage->data();
        }

    private:
        bool removeReference(const Reference& ref) noexcept
        {
            ref.onRemoveReference();
            return m_refs.erase(&ref).second;
        }

        bool onSetReference(const Reference& ref) noexcept
        {
            return m_refs.emplace(&ref).second;
        }

        bool onRemoveReference(const Reference& ref) noexcept
        {
            return m_refs.erase(&ref).second;
        }

    private:
        //! The underlying data storage.
        std::optional<Storage> m_storage;
        //! All data nodes refs referencing this data node.
        std::unordered_set<const Reference*> m_refs;
    };

    template<typename... Args>
    class LogicDataNodeRef final : public LogicDataNodeBase,
                                   public LogicDataNodeLinks<typename LogicDataNode<Args...>::Data>
    {
    public:
        using Base     = LogicDataNodeBase;
        using DataNode = LogicDataNode<Args...>;
        using Data     = typename DataNode::Data;
        static_assert(!std::is_const_v<Data>, "Only non-const type allowed!");

        friend class DataNode;

    public:
        template<typename... Args>
        LogicDataNodeRef(Args&&... args) noexcept
            : Base(std::forward<Args>(args)...)
        {
        }

        ~LogicDataNodeRef() noexcept
        {
            removeReference();
        }

        const Data& data() const noexcept
        {
            if constexpr(throwIfDataNodeNoStorage)
            {
                EXECGRAPH_LOGTHROW_IF(!hasData(),
                                      "Node data with id: {0} has no assigned storage!",
                                      id());
            }

            return *static_cast<Data&>(*Base::data());
        }
        Data& data() noexcept
        {
            return const_cast<Data&>(static_cast<const LogicDataNode&>(*this).data());
        }

    public:
        void setReference(DataNode& node) noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = node;
            node->onSetReference(*this);
        }

    public:
        bool resolveData() noexcept
        {
            if(m_node && m_node->hasData())
            {
                m_data = &m_node->data();
                return true;
            }
            return false;
        }

    private:
        void removeReference() noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = nullptr;
        }

        void onRemoveReference() noexcept
        {
            m_node = nullptr;
        }

    private:
        DataNode* m_node = nullptr;
    };

}  // namespace executionGraph