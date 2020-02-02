cmake_minimum_required(VERSION 3.12)
project(test_HashMap)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE SOURCES "test/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "src")

add_subdirectory(googletest)
target_include_directories(${PROJECT_NAME} PRIVATE "googletest/googletest/include" "googletest/googlemock/include")
target_link_libraries(${PROJECT_NAME} gmock gmock_main)
