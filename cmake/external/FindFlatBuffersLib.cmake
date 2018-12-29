# Try to find the FlatBuffer Library https://github.com/google/flatbuffers.git
include(FindPackageHandleStandardArgs)

# Try to find the library, if it is installed!
# otherwise download it
set(URL "https://github.com/google/flatbuffers.git")
set(INSTALL_DIR "${ExecutionGraph_EXTERNAL_INSTALL_DIR}/flatbuffers")

message(STATUS "flatbuffers library finding ...")
find_package(Flatbuffers QUIET PATHS ${INSTALL_DIR})

if(${USE_SUPERBUILD})

    if(NOT TARGET "flatbuffers::flatbuffers")

        message(STATUS "flatbuffers library: targer not found -> download from ${URL}")

        include(ExternalProject)
        
        ExternalProject_Add(flatbuffers
                            GIT_REPOSITORY      "${URL}"
                            GIT_TAG             master
                            GIT_SHALLOW         OFF
                            PREFIX              "${ExecutionGraph_EXTERNAL_BUILD_DIR}/flatbuffers"
                            TIMEOUT 10
                            UPDATE_DISCONNECTED  ON
                            CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
                            INSTALL_DIR "${INSTALL_DIR}")

        ExternalProject_Get_property(flatbuffers BINARY_DIR)
        message(STATUS "flatbuffers library setup -> build it!")
    endif()

else()
    if(NOT TARGET "flatbuffers::flatbuffers")
        if(${FlatBuffersLib_FIND_REQUIRED})
            message(FATAL_ERROR "flatbuffers library could not be found!")
        else()
            message(WARNING "flatbuffers library could not be found!")
        endif()
    endif()
endif()

if(TARGET "flatbuffers::flatbuffers")
    add_library(flatbuffersLib INTERFACE IMPORTED)
    set_property(TARGET flatbuffersLib PROPERTY INTERFACE_LINK_LIBRARIES flatbuffers::flatbuffers)
    set_property(TARGET flatbuffersLib PROPERTY INTERFACE_COMPILE_DEFINITIONS "$<$<CONFIG:Debug>:FLATBUFFERS_DEBUG_VERIFICATION_FAILURE>")
    message(STATUS "flatbuffers library found! Config File: ${FlatBuffers_CONFIG}")
    message(STATUS "flatbuffers library added targets: flatbufferLib")
endif()