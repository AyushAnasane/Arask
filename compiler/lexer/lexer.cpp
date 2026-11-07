
#include "lexer.hpp"
#include <cctype>
#include <unordered_map>

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
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source.substr(start, current - start);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"let", TokenType::Let},
        {"const", TokenType::Const},
        {"cout", TokenType::Cout},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"for", TokenType::For},
        {"struct", TokenType::Struct},
        {"fn", TokenType::Fn},
        {"return", TokenType::Return},
        {"true", TokenType::True},
        {"false", TokenType::False}
    };

    auto it = keywords.find(text);

    if (it != keywords.end()) {
        addToken(it->second);
    } else {
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

        case '+':
        if (peek() == '+') {
            advance();
            addToken(TokenType::PlusPlus);
        } else {
            addToken(TokenType::Plus);
        }
        break;
        case '*': addToken(TokenType::Star); break;
        case '/': addToken(TokenType::Slash); break;

        case '-':
            if (peek() == '>') {
                advance();
                addToken(TokenType::Arrow);
            } else if (peek() == '-') {
                advance();
                addToken(TokenType::MinusMinus);
            } else {
                addToken(TokenType::Minus);
            }
            break;

        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case '{': addToken(TokenType::LeftBrace); break;
        case '}': addToken(TokenType::RightBrace); break;

        case ';': addToken(TokenType::Semicolon); break;
        case ':': addToken(TokenType::Colon); break;
        case ',': addToken(TokenType::Comma); break;
        case '.': addToken(TokenType::Dot); break;

        case '=':
            if (peek() == '=') {
                advance();
                addToken(TokenType::EqualEqual);
            } else {
                addToken(TokenType::Equal);
            }
            break;

        case '!':
            if (peek() == '=') {
                advance();
                addToken(TokenType::BangEqual);
            } else {
                addToken(TokenType::Bang);
            }
            break;

        case '>':
            if (peek() == '=') {
                advance();
                addToken(TokenType::GreaterEqual);
            } else {
                addToken(TokenType::Greater);
            }
            break;

        case '<':
            if (peek() == '=') {
                advance();
                addToken(TokenType::LessEqual);
            } else {
                addToken(TokenType::Less);
            }
            break;

        case '&':
            if (peek() == '&') {
                advance();
                addToken(TokenType::AndAnd);
            } else {
                addToken(TokenType::Ampersand);
            }
            break;

        case '|':
            if (peek() == '|') {
                advance();
                addToken(TokenType::OrOr);
            } else {
                addToken(TokenType::Unknown);
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
