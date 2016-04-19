project(server)
cmake_minimum_required(VERSION 2.8)


# C++14
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

# Threads
find_package (Threads)

# Http Server Library
FILE(GLOB_RECURSE LIB_SRC_LIST "./code/*.cpp")
add_library(ekshttpserver STATIC ${LIB_SRC_LIST})

# example targets
FILE(GLOB_RECURSE INC_ALL "./code/*.h")
include_directories("code")

add_executable(${PROJECT_NAME} "./examples/ftpOverHttpServer.cpp" ${INC_ALL})

target_link_libraries(${PROJECT_NAME} ekshttpserver)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
