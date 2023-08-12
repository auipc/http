cmake_minimum_required(VERSION 3.5)
project(HTTP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")

add_definitions(-DDEBUG)

find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(glog REQUIRED)

file(GLOB server_SRC
    "src/mime.cpp"
    "src/http.cpp"
    "src/response.cpp"
    "src/main.cpp"
    "src/*.cpp"
    "src/server.cpp"
)

include_directories("src")
include_directories(${Boost_INCLUDE_DIRS})

add_executable(server ${server_SRC})
target_link_libraries(server pthread glog::glog ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_LIBRARIES})
