
#pragma once

#include <string>

namespace newlang {

enum class TokenType {

    Let,
    Const,
    Cout,
    Fn,
    Struct,
    If,
    Else,
    For,
    Return,
    True,
    False,

    Identifier,
    Number,
    String,

    Plus,
    PlusPlus,
    Minus,
    Star,
    Slash,

    Equal,
    EqualEqual,
    BangEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    Colon,
    Comma,
    Dot,
    Semicolon,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,

    Ampersand,
    Arrow,

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
