macro(print_target_properties target)

    message(STATUS "Target Properties =========")
    message(STATUS "Target: ${target}")

    get_property(RES TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
    message(STATUS "Include Dirs: ${RES}")

    get_property(RES TARGET ${target} PROPERTY LINK_LIBRARIES)
    message(STATUS "Linking with: ${RES}")

    get_property(RES TARGET ${target} PROPERTY OUTPUT_NAME)
    message(STATUS "Output name: ${RES}")

    message(STATUS "===========================")
endmacro()