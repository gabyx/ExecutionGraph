set(ExecutionGraphGUIBackend_SRCS

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
