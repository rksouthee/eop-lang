cmake_minimum_required(VERSION 3.16)

project(eop-lang CXX)

include(CTest)

if(BUILD_TESTING)
	find_package(Catch2 3 REQUIRED)
	include(Catch)
endif()

add_subdirectory(code)
