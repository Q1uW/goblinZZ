cmake_minimum_required(VERSION 3.21)
project(passanalysistest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_VERSION
        "16"
        CACHE STRING "LLVM Version")
execute_process(
        COMMAND llvm-config --cxxflags
        OUTPUT_VARIABLE LLVM_CXXFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${LLVM_CXXFLAGS}")
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_CLANG_TIDY
            "clang-tidy;--extra-arg-before=-stdlib=libc++")
    message(STATUS "Enabling clang-tidy")

    execute_process(
            COMMAND llvm-config --includedir
            OUTPUT_VARIABLE LLVM_INCLUDEDIR
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -isystem ${LLVM_INCLUDEDIR} -Wall -g -fno-rtti")
endif()
file(GLOB SAA_SRCS "src/*.cpp")
include_directories("include/")
add_library(SAAPass SHARED ${SAA_SRCS} include/AliasContext.h include/AliasInstvistor.h include/AnalyzerBase.h include/Context.h include/GlobalVistor.h include/TaintObject.h
        include/utils.h src/AliasContext.cpp)

target_link_libraries(SAAPass
        "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")


