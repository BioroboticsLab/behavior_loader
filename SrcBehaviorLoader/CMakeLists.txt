
target_include_directories(${TN} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>
)

find_package(Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(${TN} Qt5::Core)

set_target_properties(${TN} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${TN} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)
set_target_properties(${TN} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_sources(${TN}
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/PluginLoader.cpp"

)

set(${TN}_PUBLIC_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/PluginLoader.h"
)


