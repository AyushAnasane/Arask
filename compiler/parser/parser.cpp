
#include "parser.hpp"
#include <stdexcept>
#include <utility>

namespace newlang {

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens) {}

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
    if (isAtEnd()) return type == TokenType::EndOfFile;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd()) {
        statements.push_back(statement());
    }

    return statements;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::Let)) return variableDeclaration();
    if (match(TokenType::Cout)) return coutStatement();
    if (match(TokenType::For)) return forStatement();
    if (match(TokenType::If)) return ifStatement();
    if (match(TokenType::LeftBrace)) return blockStatement();

    if (check(TokenType::Identifier)) {
        if (current + 1 < tokens.size()) {
            if (tokens[current + 1].type == TokenType::PlusPlus) {
                return incrementStatement();
            }

            if (tokens[current + 1].type == TokenType::MinusMinus) {
                return decrementStatement();
            }

            if (tokens[current + 1].type == TokenType::Equal) {
                return assignmentStatement();
            }
        }
    }

    throw std::runtime_error("Unexpected statement.");
}

std::unique_ptr<Stmt> Parser::variableDeclaration() {
    std::string name = consume(
        TokenType::Identifier,
        "Expected variable name."
    ).lexeme;

    consume(TokenType::Equal, "Expected '=' after variable name.");

    auto initializer = expression();

    consume(TokenType::Semicolon, "Expected ';' after variable declaration.");

    return std::make_unique<VariableDeclaration>(
        name,
        std::move(initializer)
    );
}

std::unique_ptr<Stmt> Parser::assignmentStatement() {
    std::string name = consume(
        TokenType::Identifier,
        "Expected variable name."
    ).lexeme;

    consume(TokenType::Equal, "Expected '=' in assignment.");

    auto value = expression();

    consume(TokenType::Semicolon, "Expected ';' after assignment.");

    return std::make_unique<AssignmentStatement>(
        name,
        std::move(value)
    );
}

std::unique_ptr<Stmt> Parser::incrementStatement() {
    std::string name = consume(
        TokenType::Identifier,
        "Expected variable name."
    ).lexeme;

    consume(TokenType::PlusPlus, "Expected '++'.");

    consume(TokenType::Semicolon, "Expected ';' after increment.");

    return std::make_unique<IncrementStatement>(name);
}

std::unique_ptr<Stmt> Parser::decrementStatement() {
    std::string name = consume(
        TokenType::Identifier,
        "Expected variable name."
    ).lexeme;

    consume(TokenType::MinusMinus, "Expected '--'.");

    consume(TokenType::Semicolon, "Expected ';' after decrement.");

    return std::make_unique<DecrementStatement>(name);
}

std::unique_ptr<Stmt> Parser::updateExpression() {
    std::string name = consume(
        TokenType::Identifier,
        "Expected variable name in loop update."
    ).lexeme;

    if (match(TokenType::PlusPlus)) {
        return std::make_unique<IncrementStatement>(name);
    }

    if (match(TokenType::MinusMinus)) {
        return std::make_unique<DecrementStatement>(name);
    }

    if (match(TokenType::Equal)) {
        auto value = expression();

        return std::make_unique<AssignmentStatement>(
            name,
            std::move(value)
        );
    }

    throw std::runtime_error("Expected ++, --, or '=' in loop update.");
}

std::unique_ptr<Stmt> Parser::coutStatement() {
    consume(TokenType::LeftParen, "Expected '(' after cout.");

    auto value = expression();

    consume(TokenType::RightParen, "Expected ')' after cout expression.");
    consume(TokenType::Semicolon, "Expected ';' after cout.");

    return std::make_unique<CoutStatement>(std::move(value));
}

std::unique_ptr<Stmt> Parser::blockStatement() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        statements.push_back(statement());
    }

    consume(TokenType::RightBrace, "Expected '}' after block.");

    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LeftParen, "Expected '(' after if.");

    auto condition = expression();

    consume(TokenType::RightParen, "Expected ')' after condition.");
    consume(TokenType::LeftBrace, "Expected '{' before if body.");

    auto thenBranch = blockStatement();

    std::unique_ptr<Stmt> elseBranch = nullptr;

    if (match(TokenType::Else)) {
        consume(TokenType::LeftBrace, "Expected '{' before else body.");
        elseBranch = blockStatement();
    }

    return std::make_unique<IfStatement>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch)
    );
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LeftParen, "Expected '(' after for.");
    consume(TokenType::Let, "Expected 'let' in for loop.");

    std::string variable = consume(
        TokenType::Identifier,
        "Expected loop variable."
    ).lexeme;

    consume(TokenType::Equal, "Expected '=' after loop variable.");

    auto start = expression();

    if (match(TokenType::Comma)) {
        bool multipleInitializers =
            check(TokenType::Identifier) &&
            current + 1 < tokens.size() &&
            tokens[current + 1].type == TokenType::Equal;

        if (!multipleInitializers) {
            return rangeForStatement(
                std::move(variable),
                std::move(start)
            );
        }
    }

    std::vector<std::unique_ptr<Stmt>> initializers;

    initializers.push_back(
        std::make_unique<VariableDeclaration>(
            variable,
            std::move(start)
        )
    );

    while (previous().type == TokenType::Comma) {
        std::string name = consume(
            TokenType::Identifier,
            "Expected variable name in loop initializer."
        ).lexeme;

        consume(TokenType::Equal, "Expected '=' after variable name.");

        auto value = expression();

        initializers.push_back(
            std::make_unique<VariableDeclaration>(
                name,
                std::move(value)
            )
        );

        if (!match(TokenType::Comma)) break;
    }

    consume(TokenType::Semicolon, "Expected ';' after loop initializer.");

    auto condition = expression();

    consume(TokenType::Semicolon, "Expected ';' after loop condition.");

    std::vector<std::unique_ptr<Stmt>> increments;

    if (!check(TokenType::RightParen)) {
        do {
            increments.push_back(updateExpression());
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen, "Expected ')' after for clauses.");
    consume(TokenType::LeftBrace, "Expected '{' before loop body.");

    auto body = blockStatement();

    return std::make_unique<ForStatement>(
        std::move(initializers),
        std::move(condition),
        std::move(increments),
        std::move(body)
    );
}

std::unique_ptr<Stmt> Parser::rangeForStatement(
    std::string variable,
    std::unique_ptr<Expr> start
) {
    bool inclusive = match(TokenType::Equal);

    auto end = expression();

    std::unique_ptr<Expr> step = nullptr;

    if (match(TokenType::Comma)) {
        step = expression();
    }

    consume(TokenType::RightParen, "Expected ')' after range.");
    consume(TokenType::LeftBrace, "Expected '{' before loop body.");

    auto body = blockStatement();

    return std::make_unique<RangeForStatement>(
        variable,
        std::move(start),
        std::move(end),
        std::move(step),
        inclusive,
        std::move(body)
    );
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual)) {
        std::string op = previous().lexeme;
        auto right = comparison();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (
        match(TokenType::Greater) ||
        match(TokenType::GreaterEqual) ||
        match(TokenType::Less) ||
        match(TokenType::LessEqual)
    ) {
        std::string op = previous().lexeme;
        auto right = term();

        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
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

    while (match(TokenType::Star) || match(TokenType::Slash)) {
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
    if (match(TokenType::Minus) || match(TokenType::Plus)) {
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
        return std::make_unique<VariableExpr>(previous().lexeme);
    }

    if (match(TokenType::LeftParen)) {
        auto expr = expression();

        consume(TokenType::RightParen, "Expected ')' after expression.");

        return expr;
    }

    throw std::runtime_error("Expected expression.");
}

}
