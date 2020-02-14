include(GNUInstallDirs)

cmake_minimum_required(VERSION 3.15)

project(virtual-terminal LANGUAGES CXX)

add_library(virtual_terminal
	src/virtual_terminal.cpp
	include/virtual_terminal.hpp
)

target_compile_features(virtual_terminal PUBLIC cxx_std_17)

target_include_directories(virtual_terminal
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(virtual_terminal PROPERTIES
	PUBLIC_HEADER
		include/virtual_terminal.hpp)

add_executable(program
	prg/main.cpp
)

target_link_libraries(program PUBLIC virtual_terminal)


install(TARGETS virtual_terminal EXPORT Vc)
install(EXPORT Vc DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VirtualTerminal NAMESPACE vt:: FILE VirtualTerminalConfig.cmake)
