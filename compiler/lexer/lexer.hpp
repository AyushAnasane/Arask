
#pragma once

#include <string>
#include <vector>
#include "newlang/token.hpp"

namespace newlang {

class Lexer {
private:
    std::string source;
    std::vector<Token> tokens;

    int start = 0;
    int current = 0;

    int line = 1;
    int column = 1;

    int startLine = 1;
    int startColumn = 1;

    bool isAtEnd();
    char advance();
    char peek();

    void addToken(TokenType type);
    void scanToken();
    void identifier();
    void number();

public:
    explicit Lexer(const std::string& sourceCode);

    std::vector<Token> scanTokens();
};

}
