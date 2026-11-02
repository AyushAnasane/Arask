
#include "lexer.hpp"
#include <cctype>


namespace newlang {

Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode) {}

bool Lexer::isAtEnd() {
    return current >= static_cast<int>(source.length());
}

char Lexer::advance() {
    char c = source[current++];
    column++;
    return c;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);

    tokens.push_back({
        type,
        text,
        line,
        column - static_cast<int>(text.length())
    });
}

void Lexer::identifier() {
    while (std::isalnum(
        static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);

    if (text == "let") {
        addToken(TokenType::Let);
    }
    else if (text == "print") {
        addToken(TokenType::Print);
    }
    else {
        addToken(TokenType::Identifier);
    }
}

void Lexer::number() {
    while (std::isdigit(
        static_cast<unsigned char>(peek()))) {
        advance();
    }

    addToken(TokenType::Number);
}

void Lexer::scanToken() {
    char c = advance();

    switch (c) {
        case ' ': case '\r': case '\t':
            break;

        case '\n':
            line++;
            column = 1;
            break;

        case '+': addToken(TokenType::Plus); break;
        case '-': addToken(TokenType::Minus); break;
        case '*': addToken(TokenType::Star); break;
        case '/': addToken(TokenType::Slash); break;
        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case ';': addToken(TokenType::Semicolon); break;

        case '=':
            if (peek() == '=') {
                advance();
                addToken(TokenType::EqualEqual);
            } else {
                addToken(TokenType::Equal);
            }
            break;

        default:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                number();
            }
            else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                identifier();
            }
            else {
                addToken(TokenType::Unknown);
            }
            break;
    }
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({
        TokenType::EndOfFile,
        "",
        line,
        column
    });

    return tokens;
}

}
