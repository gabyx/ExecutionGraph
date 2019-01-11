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
#include <stack>
#include "executionGraphGui/backend/ExecutionGraphBackend.hpp"
#include "executionGraphGui/backend/requestHandlers/RequestHandlerCommon.hpp"
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
    flatbuffers::Offset<sG::PathInfo>
    collectPathInfo(Builder& builder,
                    const std::path& root,
                    std::size_t maxLevel = 1)
    {
        flatbuffers::Offset<sG::PathInfo> resOff;

        // Directory structure for depth-first traversal.
        struct Directory
        {
            std::filesystem::directory_entry entry;
            std::size_t parentIndex = 0;  //!< Index into dirInfos
            Directory* parent       = nullptr;

            std::size_t size = 0;  //! Directory size.

            // if directory
            std::vector<flatbuffers::Offset<sG::PathInfo>> dirInfos;
            std::vector<flatbuffers::Offset<sG::PathInfo>> fileInfos;

            bool visited = false;
        };

        //! Get the statistics of the path entry `e`.
        auto getStats = [](auto& builder, auto& e) {
            auto fTime      = e.last_write_time();
            std::time_t t   = decltype(fTime)::clock::to_time_t(fTime);
            auto offModTime = builder.CreateString(std::asctime(std::localtime(&t)));

            sG::Permissions per;
            const auto perm = e.status().permissions();
            using fp        = std::filesystem::perms;
            if((perm & (fp::owner_read | fp::owner_write)) != fp::none)
            {
                per = sG::Permissions::Permissions_OwnerReadWrite;
            }
            else if((perm & fp::owner_read) != fp::none)
            {
                per = sG::Permissions::Permissions_OwnerRead;
            }
            else if((perm & fp::owner_write) != fp::none)
            {
                per = sG::Permissions::Permissions_OwnerWrite;
            }
            return std::make_tuple(per, offModTime);
        };

        //! Serialize file info.
        auto buildFileInfo = [&getStats](auto& builder, const auto& e) {
            EXECGRAPHGUI_BACKENDLOG_TRACE("Build file info: '{0}'", e.path());
            auto t = getStats(builder, e);
            return sG::CreatePathInfo(builder,
                                      builder.CreateString(e.path().native()),
                                      builder.CreateString(e.path().filename().native()),
                                      std::get<0>(t),
                                      e.file_size(),
                                      std::get<1>(t),
                                      true);
        };

        // Serialize directory info.
        auto buildDirectoryInfo = [&getStats](auto& builder,
                                              const auto& e,
                                              Directory* d = nullptr) {
            EXECGRAPHGUI_BACKENDLOG_TRACE("Build dir info: '{0}'", e.path());
            auto t = getStats(builder, e);
            return sG::CreatePathInfo(builder,
                                      builder.CreateString(e.path().native()),
                                      builder.CreateString(e.path().filename().native()),
                                      std::get<0>(t),
                                      0,
                                      std::get<1>(t),
                                      false,
                                      d ? builder.CreateVector(d->fileInfos) : 0,
                                      d ? builder.CreateVector(d->dirInfos) : 0);
        };

        //! Add all contained subdirectories and as well serialize file infos.
        auto visit = [&](auto& stack, auto& data, bool recurse) {
            std::size_t dirs = 0;
            for(auto& e : std::filesystem::directory_iterator(data.entry.path()))
            {
                if(e.is_directory())
                {
                    if(recurse)
                    {
                        stack.emplace(Directory{e, dirs /*offset index*/, &data});  // No invalidation of &Directory at reallocation!
                    }
                    else
                    {
                        // build file info and store offset!
                        auto off = buildDirectoryInfo(builder, e);
                        data.dirInfos.emplace_back(off);
                    }
                    ++dirs;
                }
                else if(e.is_regular_file())
                {
                    // build file info and store offset!
                    auto off = buildFileInfo(builder, e);
                    data.size += e.file_size();
                    data.fileInfos.emplace_back(off);
                }
            }

            data.dirInfos.resize(dirs);
            data.visited = true;
        };

        // Early return for files
        if(std::filesystem::is_regular_file(root))
        {
            return buildFileInfo(builder, std::filesystem::directory_entry(root));
        }

        std::stack<Directory> paths;
        paths.emplace(Directory{std::filesystem::directory_entry(root)});  // Depth-first travesal due to flatbuffer! (start at the bottom)
        Directory* current;
        std::size_t currLevel = 0;

        while(paths.size())
        {
            current = &paths.top();

            bool traverseUp = current->visited;

            if(traverseUp)
            {
                // Finish current path (file or directory with no more subdirs)
                auto off = buildDirectoryInfo(builder, current->entry, current);
                // Set the offset to this info in the parent, such that it can build its structures later!
                if(current->parent)
                {
                    current->parent->dirInfos[current->parentIndex] = off;
                }
                else
                {
                    resOff = off;
                    break;
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

        return resOff;
    }
}  // namespace

//! Init the function mapping.
FileBrowserRequestHandler::FuncMap FileBrowserRequestHandler::initFunctionMap()
{
    using Entry = typename FuncMap::Entry;

    auto r = {Entry{targetBase / "files/browse",
                    Function{&FileBrowserRequestHandler::handleBrowse}}};
    return {r};
}

//! Static handler map: request to handler function mapping.
const FileBrowserRequestHandler::FuncMap
    FileBrowserRequestHandler::m_functionMap = FileBrowserRequestHandler::initFunctionMap();

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
const std::unordered_set<FileBrowserRequestHandler::HandlerKey>&
FileBrowserRequestHandler::requestTargets() const
{
    return m_functionMap.keys();
}

//! Handle the request.
void FileBrowserRequestHandler::handleRequest(const Request& request,
                                              ResponsePromise& response)
{
    EXECGRAPHGUI_BACKENDLOG_INFO("FileBrowserRequestHandler::handleRequest");
    m_functionMap.dispatch(request.target().native(), *this, request, response);
}

//! Handle the operation of getting all path information on a path.
void FileBrowserRequestHandler::handleBrowse(const Request& request,
                                             ResponsePromise& response)
{
    using Allocator = ResponsePromise::Allocator;

    // Request validation
    auto& payload = request.payload();
    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(payload == std::nullopt,
                                      "Request Directory is null!");

    auto browseReq = getRootOfPayloadAndVerify<sG::BrowseRequest>(*payload);

    std::path root = browseReq->path()->c_str();

    if(root.is_relative())
    {
        root = m_rootPath / root;
    }
    root = std::filesystem::canonical(root);

    EXECGRAPHGUI_THROW_BAD_REQUEST_IF(!std::filesystem::exists(root),
                                      "FileBrowse path {0} does not exist!",
                                      root);

    AllocatorProxyFlatBuffer<Allocator> allocator(response.getAllocator());
    flatbuffers::FlatBufferBuilder builder(1024, &allocator);

    auto off = sG::CreateBrowseResponse(builder,
                                        collectPathInfo(builder, root, browseReq->recursive()));
    builder.Finish(off);

    // Set the response.
    response.setReady(ResponsePromise::Payload{releaseIntoBinaryBuffer(std::move(allocator),
                                                                       builder),
                                               "application/octet-stream"});
}