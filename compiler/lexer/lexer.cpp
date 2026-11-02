
#include "lexer.hpp"
#include <cctype>

using namespace std;


namespace newlang {

Lexer::Lexer(const string& sourceCode)
    : source(sourceCode) {}

bool Lexer::isAtEnd() {
    return current >= static_cast<int>(source.length());
}

char Lexer::advance() {
    char c = source[current++];
     if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    return c;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void Lexer::addToken(TokenType type) {
    string text = source.substr(start, current - start);

    tokens.push_back({
        type,
        text,
        startLine,
        startColumn
    });
}

void Lexer::identifier() {
    while (isalnum(
        static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }

    string text = source.substr(start, current - start);

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
    while (isdigit(
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
            if (isdigit(static_cast<unsigned char>(c))) {
                number();
            }
            else if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
                identifier();
            }
            else {
                addToken(TokenType::Unknown);
            }
            break;
    }
}

vector<Token> Lexer::scanTokens() {

    tokens.clear();
    start = 0;
    current = 0;
    line = 1;
    column = 1;
    while (!isAtEnd()) {
        start = current;
        startLine = line;
        startColumn = column;

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
