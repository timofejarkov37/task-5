# Минимальная версия CMake и название проекта
cmake_minimum_required(VERSION 3.15)
project(replace C)

# Стандарт C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Складываем все .exe и .dll в общую папку bin (для Windows)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Настройки для MSVC (Windows)
if(MSVC)
    add_compile_options(/wd5045)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    link_libraries(msvcrt.lib)
    link_libraries(legacy_stdio_definitions.lib)
endif()

# Размер блока чтения (в байтах). Задаётся при конфигурации:
if(NOT DEFINED BLOCK_SIZE)
    set(BLOCK_SIZE 4096)
endif()
message(STATUS "BLOCK_SIZE = ${BLOCK_SIZE}")

# Опция: статическая или динамическая библиотека
option(BUILD_SHARED_LIBS "Build libraries as shared (dynamic)" OFF)

# Опция: собирать тесты
option(BUILD_TESTS "Build tests" ON)

# Подключаем подпроекты
add_subdirectory(lib)
add_subdirectory(app)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()