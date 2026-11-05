
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/semantic_analyzer.hpp"

using namespace newlang;

bool analyzeSource(
    const std::string& source,
    std::string* errorMessage = nullptr
) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.scanTokens();

        Parser parser(tokens);
        auto statements = parser.parse();

        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);

        return true;
    } catch (const std::exception& error) {
        if (errorMessage) {
            *errorMessage = error.what();
        }

        return false;
    }
}

void expectSuccess(const std::string& source) {
    std::string error;

    if (!analyzeSource(source, &error)) {
        throw std::runtime_error(
            "Expected semantic analysis to succeed.\n"
            "Source: " + source +
            "\nActual error: " + error
        );
    }
}

void expectFailure(const std::string& source) {
    if (analyzeSource(source)) {
        throw std::runtime_error("Expected semantic analysis to fail.");
    }
}

void expectFailureAt(
    const std::string& source,
    const std::string& expectedLocation
) {
    std::string error;

    if (analyzeSource(source, &error)) {
        throw std::runtime_error(
            "Expected semantic analysis to fail."
        );
    }

    if (error.find(expectedLocation) == std::string::npos) {
        throw std::runtime_error(
            "Expected diagnostic location: " + expectedLocation +
            "\nActual error: " + error
        );
    }
}

int main() {
    try {
        expectSuccess(
            "fn main() { let age: i32 = 19; cout(age); }"
        );

        expectFailure(
            "fn main() { cout(unknown); }"
        );

        expectFailure(
            "fn main() { let age = 19; let age = 20; }"
        );

        expectFailure(
            "fn main() { missing(); }"
        );

        expectFailure(
            "fn add(a: i32, b: i32) -> i32 { return a + b; }"
            "fn main() { add(10); }"
        );

        expectFailure(
            "fn main() { let age: i32 = 19; age = 5000000000; }"
        );

        expectSuccess(
            "fn getAge() -> i32 { return 19; }"
        );

        expectSuccess(
            "fn doNothing() { return; }"
        );

        expectFailure(
            "fn getAge() -> i32 { return 5000000000; }"
        );

        expectFailure(
            "fn getAge() -> i32 { return; }"
        );

        expectFailure(
            "fn getAge() { return 19; }"
        );

        expectFailure(
            "fn calculate(x: i32) -> i32 { "
            "if (x > 0) { return x; } "
            "}"
        );

        expectSuccess(
            "fn calculate(x: i32) -> i32 { "
            "if (x > 0) { return x; } "
            "else { return 0; } "
            "}"
        );

        expectFailure(
            "fn calculate(x: i32) -> i32 { "
            "if (x > 0) { return x; } "
            "else { cout(x); } "
            "}"
        );

        expectFailure(
            "fn main() { "
            "if (1) { let local = 10; } "
            "cout(local); "
            "}"
        );

        expectSuccess(
            "fn main() { "
            "let value = 10; "
            "if (1) { "
            "let value = 20; "
            "cout(value); "
            "} "
            "cout(value); "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let value = 10; "
            "let value = 20; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "for (let i = 0; i < 3; i++) { "
            "cout(i); "
            "} "
            "cout(i); "
            "}"
        );

        expectSuccess(
            "fn calculate(value: i32) -> i32 { "
            "let value = 20; "
            "return value; "
            "}"
        );

        expectFailure(
            "fn calculate(value: i32, value: i32) -> i32 { "
            "return value; "
            "}"
        );

        expectSuccess(
            "fn main() { "
            "let x = 10; "
            "if (1) { "
            "let x = 20; "
            "if (1) { let x = 30; cout(x); } "
            "cout(x); "
            "} "
            "cout(x); "
            "}"
        );


                expectSuccess(
            "fn main() { "
            "let isReady: bool = true; "
            "cout(isReady); "
            "}"
        );

        expectSuccess(
            "fn main() { "
            "let isReady: bool = true; "
            "isReady = false; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let isReady: bool = 10; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let age: i32 = 19; "
            "age = true; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let isReady: bool = true; "
            "isReady++; "
            "}"
        );

        expectSuccess(
            "fn main() { "
            "let result: bool = true == false; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let result: bool = true + false; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let result: bool = true < false; "
            "}"
        );

        expectFailure(
            "fn main() { "
            "let result: i32 = 10 + true; "
            "}"
        );

        std::cerr << "Test 33: Undefined variable location\n";
        expectFailureAt(
            "fn main() {\n"
            "    cout(unknown);\n"
            "}",
            "line 2, column 10"
        );

        std::cerr << "Test 34: Binary type mismatch location\n";
        expectFailureAt(
            "fn main() {\n"
            "    let result: i32 = 10 + true;\n"
            "}",
            "line 2, column 26"
        );

        std::cerr << "Test 35: Undefined function location\n";
        expectFailureAt(
            "fn main() {\n"
            "    missing();\n"
            "}",
            "line 2, column 5"
        );

        std::cerr << "Test 36: Duplicate variable location\n";
        expectFailureAt(
            "fn main() {\n"
            "    let age = 19;\n"
            "    let age = 20;\n"
            "}",
            "line 3, column 5"
        );

        std::cerr << "Test 37: Function argument type mismatch location\n";
        expectFailureAt(
            "fn show(value: i32) { cout(value); }\n"
            "fn main() {\n"
            "    show(true);\n"
            "}",
            "line 3, column 10"
        );

        std::cerr << "Test 38: Assignment type mismatch location\n";
        expectFailureAt(
            "fn main() {\n"
            "    let age: i32 = 19;\n"
            "    age = true;\n"
            "}",
            "line 3, column 11"
        );

        std::cerr << "Test 39: Return type mismatch location\n";
        expectFailureAt(
            "fn getValue() -> i32 {\n"
            "    return true;\n"
            "}",
            "line 2, column 12"
        );

        std::cerr << "Test 40: Missing return path location\n";
        expectFailureAt(
            "fn getValue(x: i32) -> i32 {\n"
            "    if (x > 0) { return x; }\n"
            "}",
            "line 1, column 1"
        );

        std::cerr << "Test 41: Boolean function parameter\n";
        expectSuccess(
            "fn show(value: bool) { cout(value); }"
            "fn main() { show(true); }"
        );

        std::cerr << "Test 42: Boolean function return\n";
        expectSuccess(
            "fn isReady() -> bool { return true; }"
            "fn main() { let ready: bool = isReady(); }"
        );

        std::cerr << "Test 43: Boolean function argument mismatch\n";
        expectFailure(
            "fn show(value: bool) { cout(value); }"
            "fn main() { show(10); }"
        );

        std::cerr << "Test 44: Boolean return type mismatch\n";
        expectFailure(
            "fn isReady() -> bool { return 10; }"
        );

        std::cerr << "Test 45: Invalid for loop condition\n";
        expectFailure(
            "fn main() { "
            "for (let i = 0; true + false; i++) { cout(i); } "
            "}"
        );

        std::cerr << "Test 46: Boolean range boundary rejected\n";
        expectFailure(
            "fn main() { "
            "for (i in true..false) { cout(i); } "
            "}"
        );

        std::cerr << "Test 47: Boolean range step rejected\n";
        expectFailure(
            "fn main() { "
            "for (i in 0..10 by true) { cout(i); } "
            "}"
        );

        std::cerr << "Test 48: int return type normalization\n";

        expectSuccess(
            "fn getAge() -> int { "
            "return 19; "
            "} "
            "fn main() { "
            "let age: i32 = getAge(); "
            "}"
        );

                std::cerr << "Test 49: Duplicate parameter location\n";
        expectFailureAt(
            "fn calculate(value: i32, value: i32) -> i32 {\n"
            "    return value;\n"
            "}",
            "line 1, column 26"
        );

        std::cerr << "Test 50: Unary operator type mismatch location\n";
        expectFailureAt(
            "fn main() {\n"
            "    let result: bool = -true;\n"
            "}",
            "line 2, column 24"
        );

        std::cerr << "Test 51: Boolean function argument mismatch location\n";
        expectFailureAt(
            "fn show(value: bool) { cout(value); }\n"
            "fn main() {\n"
            "    show(10);\n"
            "}",
            "line 3, column 10"
        );

        std::cout << "All semantic tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Semantic test failed: "
                  << error.what() << '\n';
        return 1;
    }
}
