cmake_minimum_required(VERSION 3.15)

project(virtual-console LANGUAGES CXX)

add_library(virtual_console
	src/virtual_terminal.cpp
)

target_compile_features(virtual_console PUBLIC cxx_std_17)

target_include_directories(virtual_console PUBLIC include)

add_executable(program
	prg/main.cpp
)

target_link_libraries(program PUBLIC virtual_console)
