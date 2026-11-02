cmake_minimum_required(VERSION 3.20)

project(NewLang
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Lexer library
add_library(newlang_lexer
    compiler/lexer/lexer.cpp
)

target_include_directories(newlang_lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
)

# Main compiler
add_executable(newlang
    compiler/main.cpp
)

target_link_libraries(newlang PRIVATE newlang_lexer)

# Testing
enable_testing()

add_executable(lexer_tests
    tests/lexer_tests.cpp
)

target_link_libraries(lexer_tests PRIVATE newlang_lexer)

add_test(
    NAME LexerTests
    COMMAND lexer_tests
)