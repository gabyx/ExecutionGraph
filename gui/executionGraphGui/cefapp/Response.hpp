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

#ifndef cefapp_Response_hpp
#define cefapp_Response_hpp

#include <future>
#include <memory>
#include <rttr/type>
#include <string>
#include "cefapp/BinaryPayload.hpp"
#include "cefapp/BufferPool.hpp"
#include "cefapp/Loggers.hpp"

/* ---------------------------------------------------------------------------------------*/
/*!
    A move-only response promise wrapper.

    Such an instance is created by a `BackendRequestHandler`, 
    which will move it to the BackendRequestHandler.

    The `BackendRequestHandler` will extract the future for read access to the payload. 

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
    using Payload = BinaryPayload;

protected:
    ResponsePromise(std::shared_ptr<BufferPool> allocator, bool bCancelOnDestruction = false)
        : m_allocator(allocator)
        , m_bCancelOnDestruction(bCancelOnDestruction){};

    ResponsePromise(const ResponsePromise&) = delete;
    ResponsePromise& operator=(const ResponsePromise&) = delete;

    ResponsePromise(ResponsePromise&&) = default;
    ResponsePromise& operator=(ResponsePromise&&) = default;

public:
    virtual ~ResponsePromise() = default;

public:
    //! Callback for signaling that the response object is available.
    void setReady(Payload&& payload)
    {
        if(m_state != State::Nothing)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("Promise is already set to a state!");
            return;
        }
        m_state = State::Ready;
        m_promisePayload.set_value(std::move(payload));
        setReadyImpl();  // forward to actual instance
    }

    //! Callback for signaling that this request is cancled.
    void setCanceled(const std::string& reason)
    {
        if(m_state != State::Nothing)
        {
            EXECGRAPHGUI_BACKENDLOG_WARN("Promise is already set to a state!");
            return;
        }
        m_state = State::Canceled;
        m_promisePayload.set_exception(std::make_exception_ptr(std::runtime_error(reason)));
        setCanceledImpl(reason);  // forward to actual instance
    }

protected:
    virtual void setReadyImpl()                             = 0;
    virtual void setCanceledImpl(const std::string& reason) = 0;

    //! Function to be called in derived classed, which want to automatically
    //! resolve on destruction!
    virtual void setResolveOnDestruction()
    {
        if(m_state == State::Nothing)
        {
            if(m_bCancelOnDestruction)
            {
                setCanceled("Cancled promise on destruction, because of unknown reason!");
            }
        }
    }

private:
    friend class ResponseFuture;
    std::promise<Payload> m_promisePayload;  //!< Response Data which gets set in `m_responseWrite`.

    // todo: Up to now: Hand over the buffer to Dispatcher thread, it will be used in the FlatBufferBuilder
    // The dispatcher creats the flatbuffer, returns a DetachedBuffer which ends up somehow in the payload promise
    // and gets deleted (over the same instance m_allocator) in the BackendRequestHandler ...
    std::shared_ptr<BufferPool> m_allocator;  //! Thread-safe allocator which allocates a buffer for us.

    State m_state               = State::Nothing;  //!< The state of this promise
    bool m_bCancelOnDestruction = false;           //!< If the promise should be cancled on destruction.
};

/* ---------------------------------------------------------------------------------------*/
/*!
    Futre wrapper which only allows to read the response.

    @date Thu Mar 01 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponseFuture final
{
    RTTR_ENABLE()

public:
    using Payload = ResponsePromise::Payload;

public:
    ResponseFuture() = default;
    ResponseFuture(ResponsePromise& responsePromise)
        : m_payloadFuture(responsePromise.m_promisePayload.get_future()){};

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
    std::future<Payload> m_payloadFuture;
};

#endif
