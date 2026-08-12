cmake_minimum_required(VERSION 3.10)
project(CppLabs LANGUAGES CXX)

# Стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти потоки (pthread)
find_package(Threads REQUIRED)

# Программа 1: клиент с двумя потоками
add_executable(program1
    program1/main.cpp
    program1/SocketClient.cpp
    program1/string_processor.cpp
)
target_link_libraries(program1 PRIVATE Threads::Threads)

# Программа 2: TCP-сервер
add_executable(program2
    program2/main.cpp
    program2/TcpLineServer.cpp
    program2/string_processor.cpp
)
target_link_libraries(program2 PRIVATE Threads::Threads)
