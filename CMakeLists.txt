cmake_minimum_required(VERSION 3.20)
project(libev_test)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "  (for debug, do cmake -DCMAKE_BUILD_TYPE=Debug)")
endif()

# -w to inhibit all warnings.
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-g -w -std=c++17")
else()
    set(CMAKE_CXX_FLAGS "-g -w -std=c++17 -flto")
endif()

include(FetchContent)
FetchContent_Declare(
    ev
    GIT_REPOSITORY https://github.com/sjinks/libev-cmake
    GIT_TAG v4.33
    )
FetchContent_MakeAvailable(ev)

add_executable(stdin stdin.cpp)
target_link_libraries(stdin ev::ev)
add_executable(two_timers_threaded two_timers_threaded.cpp)
target_link_libraries(two_timers_threaded ev::ev)
add_executable(two_timers two_timers.cpp)
target_link_libraries(two_timers ev::ev)
add_executable(timer timer.cpp)
target_link_libraries(timer ev::ev)
add_executable(socket socket.cpp)
target_link_libraries(socket ev::ev)
add_executable(price_server price_server.cpp)
target_link_libraries(price_server ev::ev)
add_executable(price_client price_client.cpp)
target_link_libraries(price_client ev::ev)

