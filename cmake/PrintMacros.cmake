macro(print_target_properties target)

    message(STATUS "Target Properties =========")
    message(STATUS "Target: ${target}")

    get_property(RES TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
    message(STATUS "Include Dirs: ${RES}")

    get_property(RES TARGET ${target} PROPERTY LINK_LIBRARIES)
    message(STATUS "Linking with: ${RES}")

    get_property(RES TARGET ${target} PROPERTY LINK_FLAGS)
    message(STATUS "Link flags: ${RES}")

    get_property(RES TARGET ${target} PROPERTY INTERFACE_LINK_LIBRARIES)
    message(STATUS "Interface Link Libraires: ${RES}")

    get_property(RES TARGET ${target} PROPERTY RUNTIME_OUTPUT_NAME)
    message(STATUS "Runtime output name: ${RES}")

    get_property(RES TARGET ${target} PROPERTY COMPILE_FLAGS)
    message(STATUS "Compile flags: ${RES}")

    get_property(RES TARGET ${target} PROPERTY COMPILE_OPTIONS)
    message(STATUS "Compile options: ${RES}")

    get_property(RES TARGET ${target} PROPERTY OUTPUT_NAME)
    message(STATUS "Output name: ${RES}")

    message(STATUS "===========================")
endmacro()