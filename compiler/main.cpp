
#include <iostream>
#include "lexer/lexer.hpp"
using namespace std;

string tokenTypeName(newlang::TokenType type) {
    using newlang::TokenType;

    switch (type) {
    case newlang::TokenType::Let: return "LET";
    case newlang::TokenType::Const: return "CONST";
    case newlang::TokenType::Cout: return "COUT";
    case newlang::TokenType::If: return "IF";
    case newlang::TokenType::Else: return "ELSE";
    case newlang::TokenType::For: return "FOR";
    case newlang::TokenType::Struct: return "STRUCT";
    case newlang::TokenType::Fn: return "FN";
    case newlang::TokenType::Return: return "RETURN";
    case newlang::TokenType::True: return "TRUE";
    case newlang::TokenType::False: return "FALSE";

    case newlang::TokenType::Identifier: return "IDENTIFIER";
    case newlang::TokenType::Number: return "NUMBER";
    case newlang::TokenType::String: return "STRING";

    case newlang::TokenType::Plus: return "PLUS";
    case newlang::TokenType::PlusPlus: return "PLUS_PLUS";
    case newlang::TokenType::Minus: return "MINUS";
    case newlang::TokenType::Star: return "STAR";
    case newlang::TokenType::Slash: return "SLASH";

    case newlang::TokenType::Equal: return "EQUAL";
    case newlang::TokenType::EqualEqual: return "EQUAL_EQUAL";
    case newlang::TokenType::BangEqual: return "BANG_EQUAL";
    case newlang::TokenType::Greater: return "GREATER";
    case newlang::TokenType::GreaterEqual: return "GREATER_EQUAL";
    case newlang::TokenType::Less: return "LESS";
    case newlang::TokenType::LessEqual: return "LESS_EQUAL";

    case newlang::TokenType::Colon: return "COLON";
    case newlang::TokenType::Comma: return "COMMA";
    case newlang::TokenType::Semicolon: return "SEMICOLON";
    case newlang::TokenType::Dot: return "DOT";
    case newlang::TokenType::LeftParen: return "LEFT_PAREN";
    case newlang::TokenType::RightParen: return "RIGHT_PAREN";
    case newlang::TokenType::LeftBrace: return "LEFT_BRACE";
    case newlang::TokenType::RightBrace: return "RIGHT_BRACE";

    case newlang::TokenType::Ampersand: return "AMPERSAND";
    case newlang::TokenType::Arrow: return "ARROW";

    case newlang::TokenType::EndOfFile: return "EOF";
    case newlang::TokenType::Unknown: return "UNKNOWN";
}

return "UNKNOWN";

    return "UNKNOWN";
}

int main() {
    string source =
    "let age = 19;\n"
    "cout(age);";

    newlang::Lexer lexer(source);

    auto tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        cout
            << tokenTypeName(token.type)
            << " | " << token.lexeme
            << " | Line: " << token.line
            << " | Column: " << token.column
            << '\n';
    }

    return 0;
}
