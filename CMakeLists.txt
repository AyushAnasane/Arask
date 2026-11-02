cmake_minimum_required(VERSION 3.20)

project(NewLang
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(newlang
    compiler/main.cpp
    compiler/lexer/lexer.cpp
)

target_include_directories(newlang PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
)