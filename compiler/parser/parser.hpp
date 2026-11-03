
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "newlang/token.hpp"
#include "ast/ast.hpp"

namespace newlang {

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    bool isAtEnd();
    const Token& peek();
    const Token& previous();

    bool check(TokenType type);
    bool match(TokenType type);
    const Token& advance();

    const Token& consume(TokenType type, const std::string& message);

    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> variableDeclaration();
    std::unique_ptr<Stmt> assignmentStatement();
    std::unique_ptr<Stmt> incrementStatement();
    std::unique_ptr<Stmt> coutStatement();
    std::unique_ptr<Stmt> blockStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> forStatement();
    std::unique_ptr<Stmt> incrementExpression();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

public:
    explicit Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<Stmt>> parse();
};

}
