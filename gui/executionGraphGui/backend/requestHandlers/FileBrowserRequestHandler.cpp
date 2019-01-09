//! ========================================================================================
//!  ExecutionGraph
//!  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//!
//!  @date Mon Jan 15 2018
//!  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//!
//!  This Source Code Form is subject to the terms of the Mozilla Public
//!  License, v. 2.0. If a copy of the MPL was not distributed with this
//!  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! ========================================================================================

#include "executionGraphGui/backend/requestHandlers/FileBrowserRequestHandler.hpp"
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/common/AllocatorProxyFlatBuffer.hpp"
#include "executionGraphGui/common/Exception.hpp"
#include "executionGraphGui/common/Loggers.hpp"
#include "executionGraphGui/messages/schemas/cpp/FileBrowserMessages_generated.h"

using namespace executionGraph;
namespace sG = executionGraphGui::serialization;

namespace
{
    //! Collect all path info starting from root with depth `maxLevel`.
    template<typename Builder>
    void collectPathInfo(Builder& builder,
                         const std::path& root,
                         std::size_t maxLevel = 1)
    {
        // Data strcture for depth-first traversal.
        struct Data
        {
            std::filesystem::directory_entry entry;
            std::size_t parentIndex;  //!< Index into dirInfos
            Data* parent = nullptr;

            // if directory
            std::vector<flatbuffers::Offset<sG::PathInfo>> dirInfos;
            std::vector<flatbuffers::Offset<sG::PathInfo>> fileInfos;

            bool visited = false;
        };

        //! Get the statistics of the path entry `e`.
        auto getStats = [](auto& builder, auto& e) {
            std::time_t t   = std::chrono::system_clock::to_time_t(e.last_write_time());
            auto offModTime = builder.CreateString(std::asctime(std::localtime(&t)));

            sG::Permissions per;
            auto& perm = e.status().permissions();
            using fp   = std::filesystem::permissions;
            if(perm & (fp::owner_read | fp::owner_write) != perms::none)
            {
                p = sG::Permissions::Permissions_OwnerReadWrite;
            }
            else if(perm & fp::owner_read != fp::none)
            {
                p = sG::Permissions::Permissions_OwnerRead;
            }
            else if(perm & fp::owner_write != fp::none)
            {
                p = sG::Permissions::Permissions_OwnerWrite;
            }
            return std::make_tuple(per, e.file_size(), offModTime);
        }

        //! Serialize file info.
        auto buildFileInfo = [](auto& builder, auto& e) {
            auto t = getStats(builder, e);
            return sG::CreatePathInfo(builder,
                                      builder.CreateString(e.path.native()),
                                      std::get<0>(t),
                                      std::uint64_t(std::get<1>(t)),
                                      std::get<2>(t));
        };

        // Serialize directory info.
        auto buildDirectoryInfo = [](auto& builder, auto& e) {
            auto t = getStats(builder, e);
            return sG::CreatePathInfo(builder,
                                      builder.CreateString(e.path.native()),
                                      std::get<0>(t),
                                      std::uint64_t(std::get<1>(t)),
                                      std::get<2>(t),
                                      e.fileInfos.size() ? builder.CreateVector(e.fileInfos) : nullptr,
                                      e.dirInfos.size() ? builder.CreateVector(e.dirInfos) : nullptr);
        };

        //! Serialize directory/file.
        auto buildInfo = [](auto& builder, auto& data) {
            if(data.entry.is_regular_file())
            {
                return buildFileInfo(builder, data.entry);
            }
            else if(data.entry.is_directory())
            {
                return buildDirectoryInfo(builder, data.entry);
            }
            else
            {
                EXECGRAPHGUI_THROW("Programming error!");
            }
        }

        std::stack<Data>
            paths = {std::filesystem::directory_entry{root}};  // Depth-first travesal due to flatbuffer! (start at the bottom)

        //! Add all contained subdirectories and as well serialize file infos.
        auto visit = [&](auto& stack, auto& data, bool recurse) {
            std::size_t dirs = 0;
            for(auto& e : fs::directory_iterator(data.path))
            {
                if(e.is_directory())
                {
                    if(recurse)
                    {
                        stack.emplace(e, dirs++ /*offset index*/, &data);  // No invalidation of &data at reallocation!
                    }
                }
                else
                {
                    auto off = buildDirectoryInfo(builder, e);
                    data.dirInfos.emplace_back(off);
                }
            }
            else if(e.is_regular_file())
            {
                // build file info and store offset!
                auto off = buildFileInfo(builder, e);
                data.fileInfos.emplace_back(off);
            }
            data.dirInfos.resize(dirs);
            data.visited = true;
        };

        Data* current;
        std::size_t currLevel = 0;

        while(paths.size())
        {
            current = &paths.top();

            bool traverseUp = current->visited;

            if(traverseUp)
            {
                // Finish current path (file or directory with no more subdirs)
                auto off = buildInfo(builder, current);
                // Set the offset to this info in the parent, such that it can build its structures later!
                if(current->parent)
                {
                    current->parent->dirInfos[current->parentIndex] = off;
                }
                else
                {
                    // we are at the root
                    return off;
                }

                paths.pop();

                if(currLevel)
                {
                    --currLevel;
                }
            }
            else
            {
                // Try traverse down (if possible) -> add subdirectories
                std::size_t currSize = paths.size();
                visit(paths, *current, currLevel < maxLevel);
                if(currSize != paths.size())
                {
                    ++currSize;
                }
            }
        }
    }
}  // namespace

//! Init the function mapping.
FunctionMap<FileBrowserRequestHandler::Function> FileBrowserRequestHandler::initFunctionMap()
{
    using Entry = typename FunctionMap<Function>::Entry;
    auto r      = {Entry{targetBase / "files/browse",
                    Function{&FileBrowserRequestHandler::handleBrowse}}};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FunctionMap<FileBrowserRequestHandler::Function> FileBrowserRequestHandler::m_functionMap = FileBrowserRequestHandler::initFunctionMap();

//! Konstructor.
FileBrowserRequestHandler::FileBrowserRequestHandler(std::shared_ptr<ExecutionGraphBackend> backend,
                                                     std::path rootPath,
                                                     const IdNamed& id)
    : BackendRequestHandler(id)
    , m_backend(backend)
    , m_rootPath(rootPath)
{
}

//! Get the request types for which this handler is registered.
const std::unordered_set<std::string>& FileBrowserRequestHandler::getRequestTypes() const
{
    return m_functionMap.keys();
}

//! Handle the request.
void FileBrowserRequestHandler::handleRequest(const Request& request,
                                              ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("FileBrowserRequestHandler::handleRequest");
    dispatch(m_functionMap.map(), request, response);
}

//! Handle the operation of getting all path information on a path.
void FileBrowserRequestHandler::handleBrowse(const Request& request,
                                             ResponsePromise& response)
{
    using Allocator = ResponsePromise::Allocator;

    // Request validation
    auto& payload = request.getPayload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request data is null!");

    auto browseReq = getRootOfPayloadAndVerify<sG::BrowseRequest>(*payload);

    const std::path root = browseReq.path().c_str();

    if(root.is_relative())
    {
        root = m_rootPath / root;
    }

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!root.exists(),
                                      "FileBrowse path {0} does not exist!",
                                      root);

    const bool recursive = browseReq.recursive();

    AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
    flatbuffers::FlatBufferBuilder builder(1024, &allocator);

    auto off = sG::CreateBrowseFileResponse(builder,
                                            collectPathInfo(root, builder, recursive););
    builder.Finish(off);

    // Set the response.
    response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                       builder),
                                               "application/octet-stream"});
}