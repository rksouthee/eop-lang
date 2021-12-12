cmake_minimum_required(VERSION 3.16)

project(eop-lang CXX)

enable_testing()
include(CTest)

# find_package(Catch2 3 REQUIRED)
add_subdirectory(thirdparty/Catch2)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/extras)
include(Catch)


add_subdirectory(code)
