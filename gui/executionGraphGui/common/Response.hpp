// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Thu Feb 22 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraphGui_common_Response_hpp
#define executionGraphGui_common_Response_hpp

#include <executionGraph/common/Identifier.hpp>
#include <future>
#include <memory>
#include <rttr/type>
#include <string>
#include "common/BinaryPayload.hpp"
#include "common/BufferPool.hpp"
#include "common/Loggers.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A move-only response promise wrapper.

    Such an instance is created by a `BackendResourceHandler`, 
    which will move it to the BackendRequestHandler.

    The `BackendResourceHandler` will extract the future for read access to the payload
    once the `ResponsePromise` has been resolved. 

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponsePromise
{
    RTTR_ENABLE()

private:
    enum class State
    {
        Nothing,
        Ready,
        Canceled
    };

public:
    using Payload   = BinaryPayload;
    using Allocator = BufferPool;
    using Id        = executionGraph::Id;

protected:
    ResponsePromise(Id requestId,
                    std::shared_ptr<Allocator> allocator,
                    bool bCancelOnDestruction = true)
        : m_requestId(requestId)
        , m_allocator(allocator)
        , m_bCancelOnDestruction(bCancelOnDestruction){};

    ResponsePromise(const ResponsePromise&) = delete;
    ResponsePromise& operator=(const ResponsePromise&) = delete;

    ResponsePromise(ResponsePromise&& other)
    {
        *this = std::move(other);
    };
    ResponsePromise& operator=(ResponsePromise&& other)
    {
        m_requestId            = other.m_requestId;
        m_promisePayload       = std::move(other.m_promisePayload);
        m_allocator            = other.m_allocator;
        m_state                = other.m_state;
        m_bCancelOnDestruction = other.m_bCancelOnDestruction;

        // dont do anything in the moved-from object.
        other.m_bCancelOnDestruction = false;
        return *this;
    };

public:
    virtual ~ResponsePromise() = default;

public:
    //! Callback for signaling that the response object is available.
    void setReady(Payload&& payload)
    {
        if(m_state != State::Nothing)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("ResponsePromise for request id: '{0}', is already set to a state!", m_requestId.getUniqueName());
            return;
        }
        m_state = State::Ready;
        m_promisePayload.set_value(std::move(payload));
        setReadyImpl();  // forward to actual instance
    }

    //! Callback for signaling that this request is cancled.
    void setCanceled(std::exception_ptr exception)
    {
        if(m_state != State::Nothing)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("ResponsePromise for request id: '{0}', is already set to a state!", m_requestId.getUniqueName());
            return;
        }
        m_state = State::Canceled;
        m_promisePayload.set_exception(exception);
        setCanceledImpl(exception);  // forward to actual instance
    }

    bool isResolved() { return m_state != State::Nothing; }

    //! Return the allocator for allocating the payload.
    auto getAllocator() { return m_allocator; }

protected:
    virtual void setReadyImpl()                                = 0;
    virtual void setCanceledImpl(std::exception_ptr exception) = 0;

    //! Function to be called in derived classed, which want to automatically
    //! resolve on destruction!
    virtual void setResolveOnDestruction()
    {
        if(!isResolved())
        {
            if(m_bCancelOnDestruction)
            {
                EXECGRAPHGUI_BACKENDLOG_WARN("ResponsePromise for request id: '{0}', has not been resolved. It will be cancled!", m_requestId.getUniqueName());
                setCanceled(std::make_exception_ptr(std::runtime_error("Cancled promise on destruction, because not handled properly!")));
            }
        }
    }

private:
    friend class ResponseFuture;
    Id m_requestId;                          //! The id of the corresponding request.
    std::promise<Payload> m_promisePayload;  //!< Response Data which gets set in `setReady`.

    // todo: Up to now: Hand over the buffer to Dispatcher thread, it will be used in the FlatBufferBuilder
    // The dispatcher creats the flatbuffer, returns a DetachedBuffer which ends up somehow in the payload promise
    // and gets deleted (over the same instance m_allocator) in the BackendRequestHandler ...
    std::shared_ptr<Allocator> m_allocator;  //! Thread-safe allocator which allocates a buffer for us.

    State m_state               = State::Nothing;  //!< The state of this promise
    bool m_bCancelOnDestruction = false;           //!< If the promise should be cancled on destruction.
};

/* ---------------------------------------------------------------------------------------*/
/*!
    Future wrapper which only allows to read the response.

    @date Thu Mar 01 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponseFuture final
{
    RTTR_ENABLE()

public:
    using Payload = ResponsePromise::Payload;
    using Id      = ResponsePromise::Id;

public:
    ResponseFuture() = default;
    ResponseFuture(ResponsePromise& responsePromise)
        : m_requestId(responsePromise.m_requestId)
        , m_payloadFuture(responsePromise.m_promisePayload.get_future()){};

    //! Copy forbidden
    ResponseFuture(const ResponseFuture&) = delete;
    ResponseFuture& operator=(const ResponseFuture&) = delete;

    //! Move allowed
    ResponseFuture(ResponseFuture&&) = default;
    ResponseFuture& operator=(ResponseFuture&&) = default;

    ~ResponseFuture() = default;

public:
    auto& getFuture() { return m_payloadFuture; }

private:
    Id m_requestId;  //!< The id of the corresponding request.
    std::future<Payload> m_payloadFuture;
};

#endif
