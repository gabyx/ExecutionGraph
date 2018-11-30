
find_package(CEFLib REQUIRED) # our own script

## Use CEF ################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "${CMAKE_MODULE_PATH}")
find_package(CEF REQUIRED)

print_cef_config()
# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
# Logical target used to link the libcef library.
add_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

message(STATUS "CEF Include Path: ${CEF_INCLUDE_PATH}")
list(APPEND INC_DIRS "${CEF_INCLUDE_PATH}/include" "${CEF_INCLUDE_PATH}")
set_target_properties(libcef_lib PROPERTIES 
                                 INTERFACE_INCLUDE_DIRECTORIES "${INC_DIRS}")

add_library(cefLib INTERFACE IMPORTED)
set_property(TARGET cefLib PROPERTY INTERFACE_LINK_LIBRARIES libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

print_target_properties("libcef_lib")
print_target_properties("libcef_dll_wrapper")
message(STATUS "cef standart libs: ${CEF_STANDARD_LIBS}")
###########################################################

######################
# Source files.
######################

# App Sources
# -------------
set(ExecutionGraphGui_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/App.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/App.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/AppCLArgs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/AppCLArgs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/AppHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/AppHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/ClientSchemeHandlerFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/ClientSchemeHandlerFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/BackendSchemeHandlerFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/BackendSchemeHandlerFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/BackendResourceHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/BackendResourceHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/common/Loggers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/Loggers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/BinaryBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/BinaryPayload.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/BufferPool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/RequestDispatcher.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/Request.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/Response.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/BackendRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/BackendRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/BackendFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/BackendFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/DummyRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/DummyRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GeneralInfoRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GeneralInfoRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GraphManipulationRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GraphManipulationRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GraphManagementRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/requestHandlers/GraphManagementRequestHandler.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/backend/ExecutionGraphBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/ExecutionGraphBackend.cpp
)
set(ExecutionGraphGui_SRCS_LINUX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/AppLinux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/Handlerlinux.cpp
)
set(ExecutionGraphGui_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/AppMac.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/PlatformSpecificImplementations.mm
)
set(ExecutionGraphGui_SRCS_WINDOWS
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/App.exe.manifest
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/App.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/AppWin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/HandlerWin.cpp
)
append_platform_sources(ExecutionGraphGui_SRCS)

set(ExecutionGraphGui_RES_SRCS_WINDOWS
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resources/cefApp.ico
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resources/small.ico
)
append_platform_sources(ExecutionGraphGui_RES_SRCS)

set(ExecutionGraphGui_SRCS ${ExecutionGraphGui_SRCS} ${ExecutionGraphGui_RES_SRCS})
# -------------

# App Helper Sources
# -------------
set(ExecutionGraphGui_HELPER_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/ProcessHelperMac.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.hpp
)
append_platform_sources(ExecutionGraphGui_HELPER_SRCS)
# -------------

# App Resources.
# -------------
set(ExecutionGraphGui_RESOURCES_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/Info.plist
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/App.icns
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/English.lproj/InfoPlist.strings
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/English.lproj/MainMenu.xib
)
append_platform_sources(ExecutionGraphGui_RESOURCES_SRCS)
# -------------


######################
# Shared configuration.
######################

# Determine the target output directory based on platform and generator.
if(GEN_NINJA OR GEN_MAKEFILES)
    # By default Ninja and Make builds don't create a subdirectory named after
    # the configuration.
    set(ExecutionGraphGui_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

    # Output binaries (executables, libraries) to the correct directory.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ExecutionGraphGui_TARGET_OUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ExecutionGraphGui_TARGET_OUT_DIR})
else()
    set(ExecutionGraphGui_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
endif()


set(ExecutionGraphGui_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Configure the client application
add_subdirectory(client)


######################
# Linux configuration.
######################

if(OS_LINUX)
    message(STATUS "Configuring target: ExecutionGraphGui ...")
    # Executable target.
    add_executable(ExecutionGraphGui ${ExecutionGraphGui_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraphGui ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    # Additional Include Directories
    target_include_directories(ExecutionGraphGui PRIVATE ${ExecutionGraphGui_INC_DIRS})
    add_dependencies(ExecutionGraphGui libcef_dll_wrapper)
    target_link_libraries(ExecutionGraphGui PRIVATE
                          libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} 
                          ExecutionGraph::Core 
                          ExecutionGraphGui::Core-Dependencies)

    # Set rpath so that libraries can be placed next to the executable.
    set_target_properties(ExecutionGraphGui PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(ExecutionGraphGui PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(ExecutionGraphGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ExecutionGraphGui_TARGET_OUT_DIR})

    # Copy binary and resource files to the target output directory.
    copy_files("ExecutionGraphGui" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${ExecutionGraphGui_TARGET_OUT_DIR}")
    copy_files("ExecutionGraphGui" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${ExecutionGraphGui_TARGET_OUT_DIR}")

    # Set SUID permissions on the chrome-sandbox target.
    set_linux_suid_permissions("ExecutionGraphGui" "${ExecutionGraphGui_TARGET_OUT_DIR}/chrome-sandbox")

    print_target_properties(ExecutionGraphGui)
endif()


#########################
# Mac OS X configuration.
#########################

if(OS_MACOSX)

    set(ExecutionGraphGui_TARGET_OUTPUTNAME "ExecutionGraphGui")
    # This is hardcoded as described in <cef_types.h> browser_subprocess_path.
    set(ExecutionGraphGui_HELPER_TARGET_OUTPUTNAME "ExecutionGraphGui Helper") 

    # Output paths for the app bundles.
    set(CEF_APP "${ExecutionGraphGui_TARGET_OUT_DIR}/${ExecutionGraphGui_TARGET_OUTPUTNAME}.app")
    set(CEF_HELPER_APP "${ExecutionGraphGui_TARGET_OUT_DIR}/${ExecutionGraphGui_HELPER_TARGET_OUTPUTNAME}.app")

    # Variable referenced from Info.plist files.
    set(PRODUCT_NAME "ExecutionGraphGui")

    message(STATUS "Configuring target: executionGraphGuiHelper ...")
    # Helper executable target.
    add_executable(executionGraphGuiHelper MACOSX_BUNDLE ${ExecutionGraphGui_HELPER_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(executionGraphGuiHelper ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    # Additional Include Directories
    target_include_directories(executionGraphGuiHelper PRIVATE ${ExecutionGraphGui_INC_DIRS})
    set_target_properties(executionGraphGuiHelper    PROPERTIES
                                                     MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/helper-Info.plist"
                                                     OUTPUT_NAME "${ExecutionGraphGui_HELPER_TARGET_OUTPUTNAME}"
                                                     RUNTIME_OUTPUT_DIRECTORY "${ExecutionGraphGui_TARGET_OUT_DIR}")

    target_link_libraries(executionGraphGuiHelper PRIVATE cefLib 
                          ExecutionGraph::Core-Headers
                          fmtLib)
    
    # Fix the framework rpath in the helper executable.
    # The helper is in $app_name.app/Contents/Frameworks/$app_name Helper.app/Contents/MacOS/
    # so set rpath up to Contents/ so that the loader can find Frameworks/.
    set_target_properties(executionGraphGuiHelper PROPERTIES INSTALL_RPATH "@executable_path/../../../..")
    set_target_properties(executionGraphGuiHelper PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

    print_target_properties(executionGraphGuiHelper)

    message(STATUS "Configuring target: ExecutionGraphGui ...")
    # Main executable target.
    add_executable(ExecutionGraphGui MACOSX_BUNDLE ${ExecutionGraphGui_RESOURCES_SRCS} ${ExecutionGraphGui_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraphGui ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})

    # Additional Include Directories
    target_include_directories(ExecutionGraphGui PRIVATE ${ExecutionGraphGui_INC_DIRS})
    add_dependencies(ExecutionGraphGui executionGraphGuiHelper ${ExecutionGraphGui_CLIENT_TARGET})
    target_link_libraries(ExecutionGraphGui PRIVATE
                          cefLib 
                          ExecutionGraph::Core
                          ExecutionGraphGui::Core-Dependencies)
    set_target_properties(ExecutionGraphGui PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/Info.plist"
                                                        OUTPUT_NAME "${ExecutionGraphGui_TARGET_OUTPUTNAME}"
                                                        RUNTIME_OUTPUT_DIRECTORY "${ExecutionGraphGui_TARGET_OUT_DIR}")

    # Copy files into the main app bundle.
    add_custom_command(
        TARGET ExecutionGraphGui
        POST_BUILD
        # Copy the helper app bundle into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_HELPER_APP}"
                "${CEF_APP}/Contents/Frameworks/${ExecutionGraphGui_HELPER_TARGET_OUTPUTNAME}.app"
        # Copy the CEF framework into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
        # Copy the client application into the Contents directory.
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CEF_APP}/Contents/client"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ExecutionGraphGui_CLIENT_TARGET_OUT_DIR}"
                "${CEF_APP}/Contents/client"
        # Copy libraries into the Contents/libs folder
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CEF_APP}/Contents/libs"
        COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:ExecutionGraph::Core>" "$<TARGET_FILE:RTTR::Core>"
                "${CEF_APP}/Contents/libs"
        VERBATIM)

    # Fix the framework rpath in the main executable.
    # The main app is at $app_name.app/Contents/MacOS/$app_name
    # so set rpath up to Contents/ so that the loader can find Frameworks/.
    set_target_properties(ExecutionGraphGui PROPERTIES INSTALL_RPATH "@executable_path/..;@executable_path/../libs")
    set_target_properties(ExecutionGraphGui PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    

    # Manually process and copy over resource files.
    # The Xcode generator can support this via the set_target_properties RESOURCE
    # directive but that doesn't properly handle nested resource directories.
    # Remove these prefixes from input file paths.
    set(PREFIXES "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/")
    copy_macosx_resources("${ExecutionGraphGui_RESOURCES_SRCS}" "${PREFIXES}" "ExecutionGraphGui" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")

    # Link the dSYM file into the bundle
    # if configured in FindCEFLib.cmake
    if(NOT ${CEF_DSYM_FILE} STREQUAL "")
        add_custom_command(
            TARGET ExecutionGraphGui
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                    "${CEF_DSYM_FILE}"
                    "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.dSYM"
        )
    endif()

    print_target_properties(ExecutionGraphGui)
endif()


#######################
# Windows configuration.
#######################

if(OS_WINDOWS)
    message(STATUS "Configuring target: ExecutionGraphGui ...")

    # Executable target.
    add_executable(ExecutionGraphGui WIN32 ${ExecutionGraphGui_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraphGui ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    add_dependencies(ExecutionGraphGui libcef_dll_wrapper)
    target_link_libraries(ExecutionGraphGui PRIVATE libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ExecutionGraph::Core ExecutionGraphGui::Core-Dependencies)

    if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        target_link_libraries(ExecutionGraphGui PRIVATE cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
    endif()

    # Add the custom manifest files to the executable.
    add_windows_manifest("${CMAKE_CURRENT_SOURCE_DIR}" "ExecutionGraphGui" "exe")

    # Copy binary and resource files to the target output directory.
    copy_files("ExecutionGraphGui" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${ExecutionGraphGui_TARGET_OUT_DIR}")
    copy_files("ExecutionGraphGui" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${ExecutionGraphGui_TARGET_OUT_DIR}")

    print_target_properties(ExecutionGraphGui)
endif()
