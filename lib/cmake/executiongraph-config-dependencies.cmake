include(CMakeFindDependencyMacro)

# need to wrap in function, because otherwise variables in approxmvbb-config.cmake get overwritten 
# especially _IMPORT_PREFIX
function(define_dependencies)

    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules;${CMAKE_MODULE_PATH}")

    #find_dependency(eigenLib REQUIRED)
    find_dependency(metaLib REQUIRED)

endfunction()

define_dependencies()