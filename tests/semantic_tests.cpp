
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <limits>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/semantic_analyzer.hpp"

using namespace newlang;

bool analyzeSource(const std::string& source) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.scanTokens();

        Parser parser(tokens);
        auto statements = parser.parse();

        SemanticAnalyzer analyzer;
        analyzer.analyze(statements);

        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void expectSuccess(const std::string& source) {
    if (!analyzeSource(source)) {
        throw std::runtime_error("Expected semantic analysis to succeed.");
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

        std::cout << "All semantic tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Semantic test failed: "
                  << error.what() << '\n';
        return 1;
    }
}
