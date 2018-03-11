
######################
# Source files.
######################

# App Sources
# -------------
set(ExecutionGraphGUI_SRCS
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
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/DummyBackendRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/DummyBackendRequestHandler.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/backend/ExecutionGraphBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backend/ExecutionGraphBackend.cpp
)
set(ExecutionGraphGUI_SRCS_LINUX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/AppLinux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/Handlerlinux.cpp
)
set(ExecutionGraphGUI_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/AppMac.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/PlatformSpecificImplementations.mm
)
set(ExecutionGraphGUI_SRCS_WINDOWS
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/App.exe.manifest
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/App.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/AppWin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/HandlerWin.cpp
)
append_platform_sources(ExecutionGraphGUI_SRCS)

set(ExecutionGraphGUI_RES_SRCS_WINDOWS
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resources/cefApp.ico
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/windows/resources/small.ico
)
append_platform_sources(ExecutionGraphGUI_RES_SRCS)

set(ExecutionGraphGUI_SRCS ${ExecutionGraphGUI_SRCS} ${ExecutionGraphGUI_RES_SRCS})
# -------------

# App Helper Sources
# -------------
set(ExecutionGraphGUI_HELPER_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/ProcessHelperMac.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/SchemeHandlerHelper.hpp
)
append_platform_sources(ExecutionGraphGUI_HELPER_SRCS)
# -------------

# App Resources.
# -------------
set(ExecutionGraphGUI_RESOURCES_MAC_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/Info.plist
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/App.icns
)
append_platform_sources(ExecutionGraphGUI_RESOURCES_MAC_SRCS)
set(ExecutionGraphGUI_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/English.lproj/InfoPlist.strings
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/English.lproj/MainMenu.xib
)
append_platform_sources(ExecutionGraphGUI_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)

set(ExecutionGraphGUI_RESOURCES_SRCS ${ExecutionGraphGUI_RESOURCES_MAC_SRCS} ${ExecutionGraphGUI_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})
# -------------


######################
# Shared configuration.
######################

# Determine the target output directory based on platform and generator.
if(GEN_NINJA OR GEN_MAKEFILES)
    # By default Ninja and Make builds don't create a subdirectory named after
    # the configuration.
    set(ExecutionGraphGUI_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

    # Output binaries (executables, libraries) to the correct directory.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ExecutionGraphGUI_TARGET_OUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ExecutionGraphGUI_TARGET_OUT_DIR})
else()
    set(ExecutionGraphGUI_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>")
endif()


set(ExecutionGraphGUI_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Configure the client application
add_subdirectory(client)


######################
# Linux configuration.
######################

if(OS_LINUX)
    message(STATUS "Configuring target: ExecutionGraph-GUI ...")
    # Executable target.
    add_executable(ExecutionGraph-GUI ${ExecutionGraphGUI_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraph-GUI ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    # Additional Include Directories
    target_include_directories(ExecutionGraph-GUI PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    add_dependencies(ExecutionGraph-GUI libcef_dll_wrapper)
    target_link_libraries(ExecutionGraph-GUI libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${ExecutionGraph_LINK_TARGETS_GUI_DEP} ExecutionGraph::Core)

    # Set rpath so that libraries can be placed next to the executable.
    set_target_properties(ExecutionGraph-GUI PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(ExecutionGraph-GUI PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(ExecutionGraph-GUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ExecutionGraphGUI_TARGET_OUT_DIR})

    # Copy binary and resource files to the target output directory.
    copy_files("ExecutionGraph-GUI" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${ExecutionGraphGUI_TARGET_OUT_DIR}")
    copy_files("ExecutionGraph-GUI" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${ExecutionGraphGUI_TARGET_OUT_DIR}")

    # Set SUID permissions on the chrome-sandbox target.
    set_linux_suid_permissions("ExecutionGraph-GUI" "${ExecutionGraphGUI_TARGET_OUT_DIR}/chrome-sandbox")

    print_target_properties(ExecutionGraph-GUI)
endif()


#########################
# Mac OS X configuration.
#########################

if(OS_MACOSX)

    set(ExecutionGraphGUI_TARGET_OUTPUTNAME "ExecutionGraphGUI")
    set(ExecutionGraphGUI_HELPER_TARGET_OUTPUTNAME "ExecutionGraphGUI Helper") # This is hardcoded as described in <cef_types.h> browser_subprocess_path.

    # Output paths for the app bundles.
    set(CEF_APP "${ExecutionGraphGUI_TARGET_OUT_DIR}/${ExecutionGraphGUI_TARGET_OUTPUTNAME}.app")
    set(CEF_HELPER_APP "${ExecutionGraphGUI_TARGET_OUT_DIR}/${ExecutionGraphGUI_HELPER_TARGET_OUTPUTNAME}.app")

    # Variable referenced from Info.plist files.
    set(PRODUCT_NAME "ExecutionGraph-GUI")

    message(STATUS "Configuring target: ExecutionGraphGUIHelper ...")
    # Helper executable target.
    add_executable(ExecutionGraphGUIHelper MACOSX_BUNDLE ${ExecutionGraphGUI_HELPER_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraphGUIHelper ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    # Additional Include Directories
    target_include_directories(ExecutionGraphGUIHelper PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    set_target_properties(ExecutionGraphGUIHelper    PROPERTIES
                                                     MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/helper-Info.plist"
                                                     OUTPUT_NAME "${ExecutionGraphGUI_HELPER_TARGET_OUTPUTNAME}"
                                                     RUNTIME_OUTPUT_DIRECTORY "${ExecutionGraphGUI_TARGET_OUT_DIR}")

    target_link_libraries(ExecutionGraphGUIHelper cefLib ExecutionGraph::Core-Headers)
    
    # Fix the framework rpath in the helper executable.
    # The helper is in $app_name.app/Contents/Frameworks/$app_name Helper.app/Contents/MacOS/
    # so set rpath up to Contents/ so that the loader can find Frameworks/.
    set_target_properties(ExecutionGraphGUIHelper PROPERTIES INSTALL_RPATH "@executable_path/../../../..")
    set_target_properties(ExecutionGraphGUIHelper PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

    print_target_properties(ExecutionGraphGUIHelper)

    message(STATUS "Configuring target: ExecutionGraph-GUI ...")
    # Main executable target.
    add_executable(ExecutionGraph-GUI MACOSX_BUNDLE ${ExecutionGraphGUI_RESOURCES_SRCS} ${ExecutionGraphGUI_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraph-GUI ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})

    # Additional Include Directories
    target_include_directories(ExecutionGraph-GUI PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    add_dependencies(ExecutionGraph-GUI ExecutionGraphGUIHelper ${ExecutionGraphGUI_CLIENT_TARGET})
    target_link_libraries(ExecutionGraph-GUI cefLib ${ExecutionGraph_LINK_TARGETS_GUI_DEP} ExecutionGraph::Core)
    set_target_properties(ExecutionGraph-GUI PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/Info.plist"
                                                        OUTPUT_NAME "${ExecutionGraphGUI_TARGET_OUTPUTNAME}"
                                                        RUNTIME_OUTPUT_DIRECTORY "${ExecutionGraphGUI_TARGET_OUT_DIR}")

    # Copy files into the main app bundle.
    add_custom_command(
        TARGET ExecutionGraph-GUI
        POST_BUILD
        # Copy the helper app bundle into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_HELPER_APP}"
                "${CEF_APP}/Contents/Frameworks/${ExecutionGraphGUI_HELPER_TARGET_OUTPUTNAME}.app"
        # Copy the CEF framework into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
        # Copy the Client Application into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR}"
                "${CEF_APP}/Contents"
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CEF_APP}/Contents/libs"
        COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:ExecutionGraph::Core>" "$<TARGET_FILE:RTTR::Core>"
                "${CEF_APP}/Contents/libs"
        VERBATIM)

    # Fix the framework rpath in the main executable.
    # The main app is at $app_name.app/Contents/MacOS/$app_name
    # so set rpath up to Contents/ so that the loader can find Frameworks/.
    set_target_properties(ExecutionGraph-GUI PROPERTIES INSTALL_RPATH "@executable_path/..;@executable_path/../libs")
    set_target_properties(ExecutionGraph-GUI PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    

    # Manually process and copy over resource files.
    # The Xcode generator can support this via the set_target_properties RESOURCE
    # directive but that doesn't properly handle nested resource directories.
    # Remove these prefixes from input file paths.
    set(PREFIXES "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/")
    copy_macosx_resources("${ExecutionGraphGUI_RESOURCES_SRCS}" "${PREFIXES}" "ExecutionGraph-GUI" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")

    # Link the dSYM file into the bundle
    # if configured in FindCEFLib.cmake
    if(NOT ${CEF_DSYM_FILE} STREQUAL "")
        add_custom_command(
            TARGET ExecutionGraph-GUI
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                    "${CEF_DSYM_FILE}"
                    "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.dSYM"
        )
    endif()

    print_target_properties(ExecutionGraph-GUI)
endif()


#######################
# Windows configuration.
#######################

if(OS_WINDOWS)
    message(STATUS "Configuring target: ExecutionGraph-GUI ...")

    # Executable target.
    add_executable(ExecutionGraph-GUI WIN32 ${ExecutionGraphGUI_SRCS})
    # Compile Options
    setTargetCompileOptionsExecutionGraph(ExecutionGraph-GUI ${ExecutionGraph_USE_ADDRESS_SANITIZER} ${ExecutionGraph_USE_LEAK_SANITIZER})
    add_dependencies(ExecutionGraph-GUI libcef_dll_wrapper)
    target_link_libraries(ExecutionGraph-GUI libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${ExecutionGraph_LINK_TARGETS_GUI_DEP} ExecutionGraph::Core)

    if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        target_link_libraries(ExecutionGraph-GUI cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
    endif()

    # Add the custom manifest files to the executable.
    add_windows_manifest("${CMAKE_CURRENT_SOURCE_DIR}" "ExecutionGraph-GUI" "exe")

    # Copy binary and resource files to the target output directory.
    copy_files("ExecutionGraph-GUI" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${ExecutionGraphGUI_TARGET_OUT_DIR}")
    copy_files("ExecutionGraph-GUI" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${ExecutionGraphGUI_TARGET_OUT_DIR}")

    print_target_properties(ExecutionGraph-GUI)
endif()
