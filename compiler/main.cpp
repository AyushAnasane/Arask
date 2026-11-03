
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: arask <source_file.ara>\n";
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << argv[1] << '\n';
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    try {
        newlang::Lexer lexer(source);

        auto tokens = lexer.scanTokens();

        newlang::Parser parser(tokens);

        auto statements = parser.parse();

        newlang::Interpreter interpreter;

        interpreter.execute(statements);
    }
    catch (const std::exception& error) {
        std::cerr << "Arask Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
