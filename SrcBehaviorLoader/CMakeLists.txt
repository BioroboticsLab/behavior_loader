
set(target ${PROJECT_NAME})
add_library(${target} STATIC)

install(
    TARGETS
        ${PROJECT_NAME}
    EXPORT
        ${PROJECT_NAME}Targets
    RUNTIME DESTINATION "${INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
)

target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(${target} stdc++fs)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(${target} Qt5::Core)

set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${target} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)
set_target_properties(${target} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_sources(${target}
    PRIVATE
        PluginLoader.cpp
)

instalL_headers(
    HEADERS
        PluginLoader.h
    DESTINATION "${INSTALL_INCLUDEDIR}"
)
