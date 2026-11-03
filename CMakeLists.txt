
cmake_minimum_required(VERSION 3.20)

project(Arask
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(newlang_lexer
    compiler/lexer/lexer.cpp
)

target_include_directories(newlang_lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
)

add_library(arask_parser
    compiler/parser/parser.cpp
)

target_include_directories(arask_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler
)

target_link_libraries(arask_parser PUBLIC newlang_lexer)

add_executable(newlang
    compiler/main.cpp
)

target_link_libraries(newlang PRIVATE arask_parser)

enable_testing()

add_executable(lexer_tests
    tests/lexer_tests.cpp
)

target_link_libraries(lexer_tests PRIVATE newlang_lexer)

add_test(
    NAME LexerTests
    COMMAND lexer_tests
)

add_executable(parser_tests
    tests/parser/parser_tests.cpp
)

target_link_libraries(parser_tests PRIVATE arask_parser)

add_test(
    NAME ParserTests
    COMMAND parser_tests
)
