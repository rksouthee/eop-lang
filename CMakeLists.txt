cmake_minimum_required(VERSION 3.16)

project(eop-lang CXX)

option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
	enable_testing()
	find_package(Catch2 3 REQUIRED)
	include(Catch)
endif()

add_subdirectory(code)
