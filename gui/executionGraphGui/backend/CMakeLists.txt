set(ExecutionGraphGUIBackend_SRCS

    ${CMAKE_CURRENT_SOURCE_DIR}/Backend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BackendRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BackendRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BackendFactory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BackendFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/DummyRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/DummyRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GeneralInfoRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GeneralInfoRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GraphManipulationRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GraphManipulationRequestHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GraphManagementRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requestHandlers/GraphManagementRequestHandler.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/ExecutionGraphBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExecutionGraphBackend.cpp

    PARENT_SCOPE
)
