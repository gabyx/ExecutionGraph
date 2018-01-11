
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
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/MessageHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/MessageHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/FileSchemeHandlerFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/FileSchemeHandlerFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.hpp
)
set(ExecutionGraphGUI_SRCS_LINUX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/AppLinux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/linux/Handlerlinux.cpp
)
set(ExecutionGraphGUI_SRCS_MACOSX
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/AppMac.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/HandlerMac.mm
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
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/RendererApp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/ProcessHelperMac.cpp
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

# Target executable names.
set(CEF_TARGET "ExecutionGraphGUI")
if(OS_MACOSX)
  set(CEF_HELPER_TARGET "ExecutionGraphGUIHelper")
  set(CEF_HELPER_TARGET_OUTPUTNAME "ExecutionGraphGUI Helper") # This is hardcoded as described in <cef_types.h> browser_subprocess_path.
endif()

# Determine the target output directory.
set_cef_target_out_dir()

set(ExecutionGraphGUI_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ExecutionGraph_INC_DIRS}
    ${CEF_INCLUDE_PATH}
)

# Configure the client application
add_subdirectory(client)


######################
# Linux configuration.
######################

if(OS_LINUX)
    message(STATUS "Configuring target: ${CEF_TARGET} ...")
    # Executable target.
    add_executable(${CEF_TARGET} ${ExecutionGraphGUI_SRCS})
    # Compile Options
    set_target_compile_options_ExecutionGraph(${CEF_TARGET})
    # Additional Include Directories
    target_include_directories(${CEF_TARGET} PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
    target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    # Set rpath so that libraries can be placed next to the executable.
    set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

    # Copy binary and resource files to the target output directory.
    copy_files("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    copy_files("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

    # Set SUID permissions on the chrome-sandbox target.
    set_linux_suid_permissions("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")

    print_target_properties(${CEF_TARGET})
endif()


#########################
# Mac OS X configuration.
#########################

if(OS_MACOSX)
    # Output paths for the app bundles.
    set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")
    set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_TARGET_OUTPUTNAME}.app")

    

    # Variable referenced from Info.plist files.
    set(PRODUCT_NAME "${CEF_TARGET}")

    message(STATUS "Configuring target: ${CEF_HELPER_TARGET} ...")
    # Helper executable target.
    add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE ${ExecutionGraphGUI_HELPER_SRCS})
    # Compile Options
    set_target_compile_options_ExecutionGraph(${CEF_HELPER_TARGET})
    # Additional Include Directories
    target_include_directories(${CEF_HELPER_TARGET} PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/helper-Info.plist
        OUTPUT_NAME ${CEF_HELPER_TARGET_OUTPUTNAME}
    )
    add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
    target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    
    # Fix the framework rpath in the helper executable.
    fix_macosx_helper_framework_rpath(${CEF_HELPER_TARGET})

    print_target_properties(${CEF_HELPER_TARGET})

    message(STATUS "Configuring target: ${CEF_TARGET} ...")
    # Main executable target.
    add_executable(${CEF_TARGET} MACOSX_BUNDLE ${ExecutionGraphGUI_RESOURCES_SRCS} ${ExecutionGraphGUI_SRCS})
    # Compile Options
    set_target_compile_options_ExecutionGraph(${CEF_TARGET})
    # Additional Include Directories
    target_include_directories(${CEF_TARGET} PRIVATE ${ExecutionGraphGUI_INC_DIRS})
    add_dependencies(${CEF_TARGET} libcef_dll_wrapper "${CEF_HELPER_TARGET}" "${ExecutionGraphGUI_CLIENT_TARGET}")
    target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${CEF_TARGET} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/Info.plist
    )

    # Copy files into the main app bundle.
    add_custom_command(
        TARGET ${CEF_TARGET}
        POST_BUILD
        # Copy the helper app bundle into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_HELPER_APP}"
                "${CEF_APP}/Contents/Frameworks/${CEF_HELPER_TARGET_OUTPUTNAME}.app"
        # Copy the CEF framework into the Frameworks directory.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
                "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
        # Copy the Client Application into the Frameworks directory.
         COMMAND ${CMAKE_COMMAND} -E copy_directory
         "${ExecutionGraphGUI_CLIENT_TARGET_OUT_DIR}"
         "${CEF_APP}/Contents"
        VERBATIM
        )

    # Fix the framework rpath in the main executable.
    fix_macosx_main_framework_rpath(${CEF_TARGET})

    # Manually process and copy over resource files.
    # The Xcode generator can support this via the set_target_properties RESOURCE
    # directive but that doesn't properly handle nested resource directories.
    # Remove these prefixes from input file paths.
    set(PREFIXES "${CMAKE_CURRENT_SOURCE_DIR}/cefapp/mac/resources/")
    copy_macosx_resources("${ExecutionGraphGUI_RESOURCES_SRCS}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")

    print_target_properties(${CEF_TARGET})
endif()


#######################
# Windows configuration.
#######################

if(OS_WINDOWS)
    message(STATUS "Configuring target: ${CEF_TARGET} ...")

    # Executable target.
    add_executable(${CEF_TARGET} WIN32 ${ExecutionGraphGUI_SRCS})
    # Compile Options
    set_target_compile_options_ExecutionGraph(${CEF_TARGET})
    add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
    target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
    endif()

    # Add the custom manifest files to the executable.
    add_windows_manifest("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

    # Copy binary and resource files to the target output directory.
    copy_files("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    copy_files("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

    print_target_properties(${CEF_TARGET})
endif()
