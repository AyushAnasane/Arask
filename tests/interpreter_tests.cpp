
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace newlang;

std::string runProgram(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();

    Parser parser(tokens);
    auto statements = parser.parse();

    Interpreter interpreter;

    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    try {
        interpreter.execute(statements);
    } catch (...) {
        std::cout.rdbuf(original);
        throw;
    }

    std::cout.rdbuf(original);

    return output.str();
}

int main() {
    {
        std::string output = runProgram(
            "let a = 10;"
            "let b = 20;"
            "let result = a + b;"
            "cout(result);"
        );

        assert(output == "30\n");
    }

    {
        std::string output = runProgram(
            "let i = 0;"
            "i++;"
            "i++;"
            "i--;"
            "cout(i);"
        );

        assert(output == "1\n");
    }

    {
        std::string output = runProgram(
            "for (let i = 0; i < 3; i++) {"
            "cout(i);"
            "}"
        );

        assert(output == "0\n1\n2\n");
    }

    {
        std::string output = runProgram(
            "for (let i = 0, j = 10; i < 3; i++, j--) {"
            "cout(i);"
            "cout(j);"
            "}"
        );

        assert(output == "0\n10\n1\n9\n2\n8\n");
    }

    {
        std::string output = runProgram(
            "for (let i = 1; i < 100; i = i * 2) {"
            "cout(i);"
            "}"
        );

        assert(output == "1\n2\n4\n8\n16\n32\n64\n");
    }

    {
        std::string output = runProgram(
            "for (let i = 0, j = 100; i < 10; i = i + 2, j = j / 2) {"
            "cout(i);"
            "cout(j);"
            "}"
        );

        assert(output == "0\n100\n2\n50\n4\n25\n6\n12\n8\n6\n");
    }

    {
        std::string output = runProgram(
            "fn hidden() { cout(99); }"
            "cout(1);"
        );

        assert(output == "1\n");
    }

    {
        std::string output = runProgram(
            "fn helper() { cout(99); }"
            "fn main() {"
            "cout(10);"
            "for (let i = 0; i < 2; i++) { cout(i); }"
            "}"
            "cout(500);"
        );

        assert(output == "10\n0\n1\n");
    }

    {
        std::string output = runProgram(
            "fn main() {"
            "greet();"
            "cout(1);"
            "}"
            "fn greet() { cout(100); }"
        );

        assert(output == "100\n1\n");
    }
    {
        std::string output = runProgram(
            "fn printScore(score: i32) {"
            "cout(score);"
            "}"
            "fn main() {"
            "printScore(42);"
            "}"
        );

        assert(output=="42\n");

    }
    std::cout << "All interpreter tests passed!\n";

    return 0;
}
