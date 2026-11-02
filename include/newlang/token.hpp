
#pragma once

#include <string>

namespace newlang {

enum class TokenType {
    Let,
    Print,

    Identifier,
    Number,

    Plus,
    Minus,
    Star,
    Slash,

    Equal,
    EqualEqual,

    LeftParen,
    RightParen,
    Semicolon,

    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

}
