
#include <iostream>
#include "lexer/lexer.hpp"

std::string tokenTypeName(newlang::TokenType type) {
    using newlang::TokenType;

    switch (type) {
        case TokenType::Let: return "LET";
        case TokenType::Print: return "PRINT";
        case TokenType::Identifier: return "IDENTIFIER";
        case TokenType::Number: return "NUMBER";
        case TokenType::Plus: return "PLUS";
        case TokenType::Minus: return "MINUS";
        case TokenType::Star: return "STAR";
        case TokenType::Slash: return "SLASH";
        case TokenType::Equal: return "EQUAL";
        case TokenType::EqualEqual: return "EQUAL_EQUAL";
        case TokenType::LeftParen: return "LEFT_PAREN";
        case TokenType::RightParen: return "RIGHT_PAREN";
        case TokenType::Semicolon: return "SEMICOLON";
        case TokenType::EndOfFile: return "EOF";
        case TokenType::Unknown: return "UNKNOWN";
    }

    return "UNKNOWN";
}

int main() {
    std::string source = "let age = 19; print(age + 5);";

    newlang::Lexer lexer(source);

    auto tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout
            << tokenTypeName(token.type)
            << " | " << token.lexeme
            << " | Line: " << token.line
            << " | Column: " << token.column
            << '\n';
    }

    return 0;
}
