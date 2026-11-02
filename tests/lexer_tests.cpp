#include <iostream>
#include <cassert>

#include "lexer/lexer.hpp"

using namespace newlang;

int main() {
    std::string source =
    "let age = 19;\n"
    "print(age);";

    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    assert(tokens.size() == 11);

    assert(tokens[0].type == TokenType::Let);
    assert(tokens[1].type == TokenType::Identifier);
    assert(tokens[2].type == TokenType::Equal);
    assert(tokens[3].type == TokenType::Number);
    assert(tokens[4].type == TokenType::Semicolon);
    assert(tokens[10].type == TokenType::EndOfFile);
    assert(tokens[0].lexeme == "let");
    assert(tokens[1].lexeme == "age");
    assert(tokens[3].lexeme == "19");

    assert(tokens[0].line == 1);
    assert(tokens[0].column == 1);

    assert(tokens[1].line == 1);
    assert(tokens[1].column == 5);
    assert(tokens[5].type == TokenType::Print);
    assert(tokens[5].line == 2);
    assert(tokens[5].column == 1);

    assert(tokens[7].lexeme == "age");
    assert(tokens[7].line == 2);
    assert(tokens[7].column == 7);

    std::cout << "All lexer tests passed!\n";

    return 0;
}