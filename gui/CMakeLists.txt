
find_package(CEFLib REQUIRED) # our own script


## Use CEF ################################################
list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")
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

###########################################################

add_subdirectory(cefsimple)
add_subdirectory(executionGraphGui)