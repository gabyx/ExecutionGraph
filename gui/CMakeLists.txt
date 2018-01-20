find_package(CEF REQUIRED)

print_cef_config()
# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
# Logical target used to link the libcef library.
add_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
###########################################################

# add_subdirectory(cefsimple)
add_subdirectory(executionGraphGui)