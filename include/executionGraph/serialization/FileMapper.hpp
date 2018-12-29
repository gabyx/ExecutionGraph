// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Sat Apr 28 2018
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#ifndef executionGraph_serialization_FileMapper_h
#define executionGraph_serialization_FileMapper_h

#include <stdint.h>
#include "executionGraph/common/FileSystem.hpp"
#include "executionGraph/common/Platform.hpp"

namespace executionGraph
{
    /* ---------------------------------------------------------------------------------------*/
    /*!
    FileMapper to quickly map a file into memory.

    @date Sat Apr 28 2018
    @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
    */
    /* ---------------------------------------------------------------------------------------*/
    class EXECGRAPH_EXPORT FileMapper final
    {
    public:
        FileMapper(const std::path& filePath) noexcept(false);
        ~FileMapper();

        //! Move allowed
        FileMapper(FileMapper&& other);
        FileMapper& operator=(FileMapper&& other);

        // Copy prohibited
        FileMapper(const FileMapper&) = delete;
        FileMapper& operator=(const FileMapper&&) = delete;

        //! Get the mapped address and size of the file.
        std::pair<const uint8_t*, std::size_t> data() { return {static_cast<const uint8_t*>(m_mappedAddress),
                                                                   m_mappedBytes}; }

    private:
        void load(const std::path& filePath) noexcept(false);
        void close();

    private:
        std::path m_filePath;  //!< Mapped file path.

        std::size_t m_offset      = 0;        //!< Offset into the file, which is mapped
        std::size_t m_mappedBytes = 0;        //!< The mapped size in bytes starting from `m_offset`.
        void* m_mappedAddress     = nullptr;  //!< The pointer to the mapped part of the file.
    };

}  // namespace executionGraph

#endif