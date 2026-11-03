#include "parser.hpp"

#include <stdexcept>
#include <utility>
#include <string>

namespace newlang {

Parser::Parser(const std::vector<Token>& inputTokens)
    : tokens(inputTokens) {

    if (tokens.empty() ||
        tokens.back().type != TokenType::EndOfFile) {
        throw std::invalid_argument(
            "Parser requires an EndOfFile token."
        );
    }
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() {
    return tokens[current];
}

const Token& Parser::previous() {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return type == TokenType::EndOfFile;
    }

    return peek().type == type;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }

    return previous();
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }

    advance();
    return true;
}

const Token& Parser::consume(
    TokenType type,
    const std::string& message
) {
    if (check(type)) {
        return advance();
    }

    throw std::runtime_error(
        "Parser error at line " +
        std::to_string(peek().line) +
        ": " + message
    );
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd()) {
        statements.push_back(statement());
    }

    return statements;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::Let)) {
        return variableDeclaration();
    }

    if (match(TokenType::Cout)) {
        return coutStatement();
    }

    throw std::runtime_error(
        "Unexpected token at line " +
        std::to_string(peek().line)
    );
}

std::unique_ptr<Stmt> Parser::variableDeclaration() {
    const Token& name = consume(
        TokenType::Identifier,
        "Expected variable name."
    );

    consume(
        TokenType::Equal,
        "Expected '=' after variable name."
    );

    auto initializer = expression();

    consume(
        TokenType::Semicolon,
        "Expected ';' after variable declaration."
    );

    return std::make_unique<VariableDeclaration>(
        name.lexeme,
        std::move(initializer)
    );
}

std::unique_ptr<Stmt> Parser::coutStatement() {
    consume(
        TokenType::LeftParen,
        "Expected '(' after cout."
    );

    auto value = expression();

    consume(
        TokenType::RightParen,
        "Expected ')' after expression."
    );

    consume(
        TokenType::Semicolon,
        "Expected ';' after cout statement."
    );

    return std::make_unique<CoutStatement>(
        std::move(value)
    );
}

std::unique_ptr<Expr> Parser::expression() {
    return term();
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match(TokenType::Plus) ||
           match(TokenType::Minus)) {
        std::string op = previous().lexeme;
        auto right = factor();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match(TokenType::Star) ||
           match(TokenType::Slash)) {

        std::string op = previous().lexeme;
        auto right = unary();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match(TokenType::Minus) ||
        match(TokenType::Plus)) {

        std::string op = previous().lexeme;
        auto right = unary();

        return std::make_unique<UnaryExpr>(
            op,
            std::move(right)
        );
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExpr>(
            std::stoi(previous().lexeme)
        );
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(
            previous().lexeme
        );
    }

    if (match(TokenType::LeftParen)) {
        auto expr = expression();

        consume(
            TokenType::RightParen,
            "Expected ')' after expression."
        );

        return expr;
    }

    throw std::runtime_error(
        "Expected an expression at line " +
        std::to_string(peek().line)
    );
}

}