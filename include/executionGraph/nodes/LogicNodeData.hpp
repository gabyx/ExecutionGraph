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
    class LogicDataNodeBase
    {
    public:
        EXECGRAPH_DEFINE_TYPES();

        template<typename T>
        bool isType() const noexcept { return type() == rttr::type::get<T>(); }

        //! Cast to a logic socket of type `LogicSocketInput<T>*`.
        //! The cast fails at runtime if the data type `T` does not match!
        template<typename T>
        auto* castToType() const noexcept(!throwIfBadSocketCast)
        {
            EXECGRAPH_THROW_BADSOCKETCAST_IF(!this->template isType<T>(),
                                             "Casting socket index '{0}' with type index '{1}' into type"
                                             "'{2}' of node id: '{3}' which is wrong!",
                                             this->getIndex(),
                                             this->type(),
                                             demangle<T>(),
                                             this->parent().getId());

            return static_cast<const LogicDataNode<T>*>(this);
        }

        //! Non-const overload.
        template<typename T>
        auto* castToType() noexcept(!throwIfBadSocketCast)
        {
            return const_cast<LogicDataNode<T>*>(static_cast<LogicDataNodeBase const*>(this)->castToType<T>());
        }

        inline NodeDataId getId() const noexcept { return m_id; }
        inline void setId(NodeDataId id) noexcept { m_id = id; }

    private:
        LogicDataNodeBase(rttr::type type, NodeDataId id)
            : m_type(type)
            , m_id(id)
        {}

    private:
        const rttr::type m_type;                    //!< The type of this node node.
        const NodeDataId m_id = nodeDataIdInvalid;  //!> Id of this node.
    };

    template<typename TData>
    class DataNodeStorage
    {
    public:
        using Data = TData;

    public:
        template<typename T>
        LogicSocketData(T&& value)
            : m_data(std::forward<T>(value)) {}

        inline Data& data() { return m_data; }
        inline const Data& data() const { return m_data; }

    private:
        Data m_data;  //!< The data.
    };

    template<typename TData>
    class DataNodeStorageRef final
    {
    };

    template<typename TData,
             typename IsReferencing = meta::bool_<false>,
             typename TStorage      = void>
    class LogicDataNode final : public LogicDataNodeBase
    {
        static constexpr bool isReferencing = IsReferencing::value;

        using Data = TData;
        static_assert(!std::is_const_v<Data>, "Only non-const type allowed!");

        using InputSocket  = LogicSocketInput<Data>;
        using OutputSocket = LogicSocketOutput<Data>;

        using Storage = meta::if_<std::is_same_v<TStorage, void>,
                                  meta::if_<IsReferencing, DataNodeStorageRef<Data>, DataNodeStorage<Data>>,
                                  meta::lazy::invoke<TStorage, Data, IsReferencing>>;

    public:
        friend class InputSocket;
        friend class OutputSocket;

    public:
        template<typename... Args>
        LogicDataNode(Args&&... args) noexcept
            : LogicDataNodeBase(std::forward<Args>(args)...)
        {
        }

        ~LogicDataNode();

        bool isAllocated() { return m_storage.data() != nullptr; }

        const RawData* data() const noexcept(!throwIfDataNodeNoStorage)
        {
            EXECGRAPH_THROW_NODE_DATA_NO_STORAGE_IF(!isAllocated(),
                                                    "Node data with id: {0} has no assigned storage!",
                                                    id());
            return *m_storage.data();
        }
        RawData* data() noexcept(!throwIfDataNodeNoStorage)
        {
            return const_cast<RawData&>(static_cast<const LogicDataNode&>(*this).data());
        }

        const auto& outputs() const noexcept { return m_outputs; }
        const auto& inputs() const noexcept { return m_inputs; }

    public:
        template<std::enable_if_t<!isReferencing, int> = 0>
        void assignStorage(Storage&& storage)
        {
            m_storage = std::move(storage);
        }

        template<std::enable_if_t<isReferencing, int> = 0>
        void assignStorage(const LogicDataNode& node)
        {
            m_storage = node.storage();
        }

    private:
        auto& storage() { return m_storage; }
        const auto& storage() { return m_storage; }

        bool onAddWriteLink(const OutputSocket& output) noexcept
        {
            auto res = m_outputs.emplace(&const_cast<OutputSocket&>(output)).second;
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

    private:
        Storage m_storage;  //!< The underlying data storage.

        std::unordered_set<InputSocket*> m_inputs;    //! All inputs getting this data node.
        std::unordered_set<OutputSocket*> m_outputs;  //! All outputs writting to his data node.
    };

    template<typename TData>
    LogicDataNode<TData>::~LogicDataNode()
    {
        for(auto* output : m_outputs)
        {
            output
        }
    }
}  // namespace executionGraph