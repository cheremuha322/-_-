cmake_minimum_required(VERSION 3.10)
project(CppLabs LANGUAGES CXX)

# Используем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим pthread
find_package(Threads REQUIRED)

# === Программа 1 ===
add_executable(program1
    program1/main.cpp
    program1/SocketClient.cpp
    program1/string_processor.cpp
)
target_include_directories(program1 PRIVATE program1)
target_link_libraries(program1 PRIVATE Threads::Threads)

# === Программа 2 ===
add_executable(program2
    program2/main.cpp
    program2/TcpLineServer.cpp
    program2/string_processor.cpp
)
target_include_directories(program2 PRIVATE program2)
target_link_libraries(program2 PRIVATE Threads::Threads)
