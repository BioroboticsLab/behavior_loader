cmake_minimum_required(VERSION 3.12)

exec_program("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "describe --abbrev=0 --tags" OUTPUT_VARIABLE GIT_TAG)

cmake_policy (SET CMP0020 NEW)

message("Git tag: " ${GIT_TAG})
project(RF_behavior_loader
    VERSION ${GIT_TAG}
    LANGUAGES CXX)
	
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    option(PACKAGE_WINDOWS_MSI "Create .msi package" OFF)
endif()

include(GNUInstallDirs)

set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} CACHE PATH "Installation directory for header files")
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} CACHE STRING "Installation directory for cmake configuration files")

set(TN RF_behavior_loader)
add_library(${TN} STATIC)
set_target_properties(${TN} PROPERTIES OUTPUT_NAME rf_behavior_loader)
include(SrcBehaviorLoader/CMakeLists.txt)

install(
    TARGETS
        RF_behavior_loader
    EXPORT
        ${PROJECT_NAME}Targets
    RUNTIME DESTINATION "${INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
)
foreach(file ${RF_behavior_loader_PUBLIC_HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_LIST_DIR}/SrcBehaviorLoader" ${file})
    get_filename_component(install_path ${rel_path} DIRECTORY)
    install(FILES ${file} DESTINATION "${INSTALL_INCLUDEDIR}/${install_path}")
endforeach()

export(
    TARGETS
        RF_behavior_loader
    FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VSSugar.cmake
    DESTINATION ${INSTALL_CONFIGDIR}/Modules/${PROJECT_NAME}
)

set(CPACK_PACKAGE_VERSION ${GIT_TAG})
set(CPACK_GENERATOR "TXZ")

include(CPack)
