cmake_minimum_required(VERSION 3.0.0)

project(player VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK REQUIRED gtkmm-3.0)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

pkg_check_modules(GST REQUIRED gstreamermm-1.0)
include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARIES})
add_definitions(${GST_CFLAGS_OTHER})

file(GLOB src
    "main.cpp"
    "window.h"
    "window.cpp"
    "custom_bin.h"
    "custom_bin.cpp"
)

add_executable(player ${src})
target_link_libraries(player ${GTK_LIBRARIES})
target_link_libraries(player ${GST_LIBRARIES})
