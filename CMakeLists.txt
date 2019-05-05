cmake_minimum_required(VERSION 3.0)

project(drm-playground)

include(FeatureSummary)
include(GNUInstallDirs)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/cmake/Modules
)

add_definitions(-Wall -Wextra)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Absolute path to executable installation directory")
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Absolute path to library installation directory")
set(PLUGIN_INSTALL_DIR ${qt_plugins_dir} CACHE PATH "Absolute path to plugin installation directory")

find_package(Qt5 REQUIRED COMPONENTS
    Core
    DBus
    Gui
)

find_package(epoxy REQUIRED)
find_package(gbm REQUIRED)
find_package(libdrm REQUIRED)
find_package(libudev REQUIRED)

add_subdirectory(src)

feature_summary(WHAT ALL)
