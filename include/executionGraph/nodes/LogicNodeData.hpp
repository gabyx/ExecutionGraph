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
#include "executionGraph/nodes/LogicDataHandle.hpp"

namespace executionGraph
{
    template<typename>
    class LogicNodeData;

    class LogicNodeDataBase
    {
        template<typename>
        friend class LogicNodeData;

    public:
        EG_DEFINE_TYPES();

        auto type() const noexcept { return m_type; }

        template<typename T>
        bool isType() const noexcept { return type() == rttr::type::get<T>(); }

        //! Cast to a logic data node of type `LogicNodeData<T>*`.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto& castToType() const noexcept
        {
            if constexpr(throwIfNodeDataNoStorage)
            {
                EG_LOGTHROW_IF(!isType<T>(),
                                      "Casting node data with id '{0}' and type '{1}' into type"
                                      "'{2}' which is wrong!",
                                      id(),
                                      type(),
                                      rttr::type::get<T>().get_name());
            }

            return static_cast<const LogicNodeData<T>&>(*this);
        }

        //! Non-const overload.
        template<typename T>
        auto& castToType() noexcept
        {
            return const_cast<LogicNodeData<T>&>(
                static_cast<const LogicNodeDataBase*>(this)->castToType<T>());
        }

        NodeDataId id() const noexcept { return m_id; }
        void setId(NodeDataId id) noexcept { m_id = id; }

    protected:
        LogicNodeDataBase(rttr::type type,
                          NodeDataId id)
            : m_type(type)
            , m_id(id)
        {}

        virtual ~LogicNodeDataBase() = default;

    private:
        const rttr::type m_type;              //!< The type of this node.
        NodeDataId m_id = nodeDataIdInvalid;  //!< Id of this node.
    };

    template<typename TData>
    class LogicNodeDataLinks
    {
    public:
        using Data         = TData;
        using InputSocket  = LogicSocketInput<Data>;
        using OutputSocket = LogicSocketOutput<Data>;

    protected:
        LogicNodeDataLinks() noexcept = default;

    public:
        ~LogicNodeDataLinks() noexcept
        {
            // @todo comment in
            // for(auto* output : m_outputs)
            // {
            //     output->onRemoveWriteLink(*this);
            // }
            // for(auto* input : m_inputs)
            // {
            //     input->onRemoveGetLink(*this);
            // }
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
    class LogicNodeDataRef;

    template<typename TData>
    class LogicNodeData final : public LogicNodeDataBase,
                                public LogicNodeDataLinks<TData>
    {
    public:
        using Base = LogicNodeDataBase;
        using Data = TData;

        using DataHandle      = LogicDataHandle<Data>;
        using DataHandleConst = LogicDataHandle<const Data>;

        static_assert(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                      "Only non-const non-reference types allowed!");

        using Reference    = LogicNodeDataRef<Data>;
        using InputSocket  = typename LogicNodeDataLinks<Data>::InputSocket;
        using OutputSocket = typename LogicNodeDataLinks<Data>::OutputSocket;

        friend Reference;
        friend InputSocket;
        friend OutputSocket;

    public:
        template<typename... Args>
        LogicNodeData(NodeDataId id,
                      Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), id)
            , m_data{std::forward<Args>(args)...}
        {
        }

        ~LogicNodeData() noexcept
        {
            for(auto* ref : m_refs)
            {
                removeReference(*ref);
            }
        };

        auto cdata() const noexcept
        {
            return DataHandleConst{m_data};
        }

        auto data() const noexcept
        {
            return cdata();
        }
        auto data() noexcept
        {
            return DataHandle{m_data};
        }

    private:
        bool removeReference(const Reference& ref) noexcept
        {
            const_cast<Reference&>(ref).onRemoveReference();
            return m_refs.erase(&ref) > 0;
        }

        bool onSetReference(const Reference& ref) noexcept
        {
            return m_refs.emplace(&ref).second;
        }

        bool onRemoveReference(const Reference& ref) noexcept
        {
            return m_refs.erase(&ref) > 0;
        }

    private:
        //! The underlying data storage.
        Data m_data;
        //! All data nodes refs referencing this data node.
        std::unordered_set<const Reference*> m_refs;
    };

    template<typename... TArgs>
    class LogicNodeDataRef final : public LogicNodeDataBase,
                                   public LogicNodeDataLinks<typename LogicNodeData<TArgs...>::Data>
    {
    public:
        using Base     = LogicNodeDataBase;
        using NodeData = LogicNodeData<TArgs...>;
        using Data     = typename NodeData::Data;

        using DataHandle      = typename NodeData::DataHandle;
        using DataHandleConst = typename NodeData::DataHandleConst;

        static_assert(!std::is_const_v<Data> && !std::is_reference_v<Data>,
                      "Only non-const non-reference types allowed!");

        friend NodeData;

    public:
        template<typename... Args>
        LogicNodeDataRef(Args&&... args) noexcept
            : Base(rttr::type::get<Data>(), std::forward<Args>(args)...)
        {
        }

        ~LogicNodeDataRef() noexcept
        {
            removeReference();
        }

        DataHandleConst data() const noexcept
        {
            return m_node->data();
        }
        DataHandle data() noexcept
        {
            return m_node->data();
        }

    public:
        void setReference(const NodeData& node) noexcept
        {
            if(m_node)
            {
                m_node->onRemoveReference(*this);
            }
            m_node = &const_cast<NodeData&>(node);
            m_node->onSetReference(*this);
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
        NodeData* m_node = nullptr;
    };

}  // namespace executionGraph