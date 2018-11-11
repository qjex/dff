cmake_minimum_required(VERSION 3.12)
project(dff)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(Qt5Widgets REQUIRED)

add_executable(dff main.cpp mainwindow.h mainwindow.cpp dff_utils.cpp dff_utils.h)

qt5_use_modules(dff Widgets)
include_directories("${PROJECT_SOURCE_DIR}/cmake-build-debug")
