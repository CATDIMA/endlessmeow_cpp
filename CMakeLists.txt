cmake_minimum_required(VERSION 3.15.0)
project(endlessmeow_cpp)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
    libevent
    GIT_REPOSITORY https://github.com/CATDIMA/libevent.git
    GIT_TAG master
)

FetchContent_Declare(
    libmeow
    GIT_REPOSITORY https://github.com/CATDIMA/libmeow.git
    GIT_TAG master
)

FetchContent_MakeAvailable(libmeow libevent)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC meow event)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${libevent_SOURCE_DIR}/include
    ${libevent_BINARY_DIR}/include
)
