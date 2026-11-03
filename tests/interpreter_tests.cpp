#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace newlang;

int main() {
    std::string source =
        "let a = 10; let b = 20; let result = a + b; cout(result);";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    auto statements = parser.parse();

    Interpreter interpreter;

    interpreter.execute(statements);

    std::cout << "Program execution completed successfully!\n";

    return 0;
}