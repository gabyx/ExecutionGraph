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

#ifndef cefapp_IResponse_hpp
#define cefapp_IResponse_hpp

#include <future>
#include <memory>
#include <rttr/type>
#include <string>
#include "cefapp/BufferPool.hpp"
class BinaryBuffer;
class Response;

struct ResponseData
{
    std::string m_mimeType;            //!<  Mime type of the response
    BinaryBuffer* m_buffer = nullptr;  //!<  BinaryBuffer
};

/* ---------------------------------------------------------------------------------------*/
/*!
    Proxy object which only allows to write the response.

    @date Thu Mar 01 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponseWrite final
{
    RTTR_ENABLE()

public:
    ResponseWrite(Response& response, std::shared_ptr<BufferPool> bufferPool)
        : m_r(response), m_bufferPool(bufferPool){};
    ~ResponseWrite() = default;

public:
    //! Callback for signaling that the response object is available.
    void setReady(BinaryBuffer* buffer, const std::string& mimeType)
    {
        // m_r.m_promiseData.set_value({buffer, mimeType});
        // m_r.setReady(); // forward to actual instance
    }

    //! Callback for signaling that this request is cancled.
    void setCancled(const std::string& reason)
    {
        // m_r.m_promiseData.set_exception(
        //         std::make_exception_ptr(std::runtime_exception(reason)));
        // m_r.setCancled(reason); // forward to actual instance
    }

    auto getBufferPool() { return m_bufferPool; }

private:
    Response& m_r;
    // todo: Up to now: Hand over the buffer to Dispatcher thread, it uses it for the allocator in the FlatBufferBuilder
    // the dispatcher creats the flatbuffer, returns a DetachedBuffer which ends up somehow in the ReponseData
    // and gets deleted (over the same instance m_bufferPool) in the BackendResourceHandler ...
    std::shared_ptr<BufferPool> m_bufferPool;  //! Thread-safe buffer pool which allocates a buffer for us.
};

/* ---------------------------------------------------------------------------------------*/
/*!
    Proxy Object which only allows to read the response.

    @date Thu Mar 01 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class ResponseRead final
{
    RTTR_ENABLE()

public:
    ResponseRead(Response& response)
        : m_r(response){};
    ~ResponseRead() = default;

public:
    //std::future<ResponseData> getResponseData() { return m_r.m_promiseData.get_future(); }

private:
    Response& m_r;
};

/* ---------------------------------------------------------------------------------------*/
/*!
    General Response Message

    A response is created by a `BackendResourceHandler`, which will pass it to
    the BackendMessageHandler with write access only.
    The BackendResourceHandler will only use read access. 

    @date Thu Feb 22 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
 */
/* ---------------------------------------------------------------------------------------*/
class Response
{
    RTTR_ENABLE()

protected:
    Response(std::shared_ptr<BufferPool> bufferPool)
        : m_responseWrite(*this, bufferPool), m_responseRead(*this)
    {}

public:
    virtual ~Response() = default;

    ResponseWrite& getWriteAccess();
    ResponseRead& getReadAccess() const;

protected:
    virtual void setReady()                             = 0;
    virtual void setCanceled(const std::string& reason) = 0;

private:
    friend class ResponseRead;
    friend class ResponseWrite;
    ResponseRead m_responseRead;
    ResponseWrite m_responseWrite;

private:
    std::promise<ResponseData> m_promiseData;  //!< Response Data which gets set in `m_responseWrite`.
};

#endif