cmake_minimum_required(VERSION 3.10)
project(Program2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(program2
    main.cpp
    TcpLineServer.cpp
    string_processor.cpp
)

target_link_libraries(program2 PRIVATE Threads::Threads)
