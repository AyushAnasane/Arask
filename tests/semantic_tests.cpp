
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

        std::cout << "All semantic tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Semantic test failed: "
                  << error.what() << '\n';
        return 1;
    }
}
