cmake_minimum_required(VERSION 3.26.4)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COMPILER_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Catapult Checkout" LANGUAGES CXX)
include(cmake/standard_build.cmake)

add_subdirectory(external)
include(GoogleTest)
enable_testing()

file(GLOB sources_list LIST_DIRECTORIES true src/*)
foreach(dir ${sources_list})
    if(IS_DIRECTORY ${dir} AND EXISTS "${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endforeach()


project(catapult_checkout)
