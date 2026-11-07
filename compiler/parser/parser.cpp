
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
    Token start = peek();
    std::unique_ptr<Stmt> stmt;

    if (match(TokenType::Fn)) {
        stmt = functionDeclaration();
    } else if (match(TokenType::Let)) {
        stmt = variableDeclaration();
    } else if (match(TokenType::Return)) {
        stmt = returnStatement();
    } else if (match(TokenType::Cout)) {
        stmt = coutStatement();
    } else if (match(TokenType::For)) {
        stmt = forStatement();
    } else if (match(TokenType::If)) {
        stmt = ifStatement();
    } else if (match(TokenType::LeftBrace)) {
        stmt = blockStatement();
    } else if (check(TokenType::Identifier)) {
        if (current + 1 < tokens.size()) {
            if (tokens[current + 1].type == TokenType::LeftParen) {
                stmt = functionCallStatement();
            } else if (tokens[current + 1].type == TokenType::PlusPlus) {
                stmt = incrementStatement();
            } else if (tokens[current + 1].type == TokenType::MinusMinus) {
                stmt = decrementStatement();
            } else if (tokens[current + 1].type == TokenType::Equal) {
                stmt = assignmentStatement();
            }
        }
    }

    if (!stmt) {
        throw std::runtime_error("Unexpected statement.");
    }

    stmt->location = {
        static_cast<std::size_t>(start.line),
        static_cast<std::size_t>(start.column)
    };

    return stmt;
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    std::unique_ptr<Expr> value = nullptr;

    if (!check(TokenType::Semicolon)) {
        value = expression();
    }

    consume(
        TokenType::Semicolon,
        "Expected ';' after return statement."
    );

    return std::make_unique<ReturnStatement>(
        std::move(value)
    );
}

std::unique_ptr<Stmt> Parser::functionCallStatement() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected function name."
    );

    std::string name = nameToken.lexeme;

    consume(TokenType::LeftParen, "Expected '(' after function name.");

    std::vector<std::unique_ptr<Expr>> arguments;

    if (!check(TokenType::RightParen)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen, "Expected ')' after function arguments.");
    consume(TokenType::Semicolon, "Expected ';' after function call.");

    auto statement = std::make_unique<FunctionCallStatement>(
        std::move(name),
        std::move(arguments)
    );

    statement->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return statement;
}

std::unique_ptr<Stmt> Parser::functionDeclaration() {
    Token functionToken = previous();

    Token nameToken = consume(
        TokenType::Identifier,
        "Expected function name after 'fn'."
    );

    std::string name = nameToken.lexeme;

    consume(TokenType::LeftParen, "Expected '(' after function name.");

    std::vector<Parameter> parameters;

    if (!check(TokenType::RightParen)) {
        do {
            Token parameterToken = consume(
                TokenType::Identifier,
                "Expected parameter name."
            );

            std::string parameterName = parameterToken.lexeme;

            consume(TokenType::Colon, "Expected ':' after parameter name.");

            std::string parameterType = consume(
                TokenType::Identifier,
                "Expected parameter type after ':'."
            ).lexeme;

            parameters.emplace_back(
                std::move(parameterName),
                std::move(parameterType),
                SourceLocation{
                    static_cast<std::size_t>(parameterToken.line),
                    static_cast<std::size_t>(parameterToken.column)
                }
            );
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen, "Expected ')' after parameters.");

    std::optional<std::string> returnType;

    if (match(TokenType::Arrow)) {
        returnType = consume(
            TokenType::Identifier,
            "Expected return type after '->'."
        ).lexeme;
    }

    consume(TokenType::LeftBrace, "Expected '{' before function body.");

    auto function = std::make_unique<FunctionDeclaration>(
        std::move(name),
        std::move(parameters),
        std::move(returnType),
        blockStatement()
    );

    function->location = {
        static_cast<std::size_t>(functionToken.line),
        static_cast<std::size_t>(functionToken.column)
    };

    return function;
}


std::unique_ptr<Stmt> Parser::variableDeclaration() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected variable name."
    );

    std::string name = nameToken.lexeme;

    std::optional<std::string> typeName;

    if (match(TokenType::Colon)) {
        typeName = consume(
            TokenType::Identifier,
            "Expected type name after ':'."
        ).lexeme;
    }

    consume(TokenType::Equal, "Expected '=' after variable declaration.");

    auto initializer = expression();

    consume(TokenType::Semicolon, "Expected ';' after variable declaration.");

    auto declaration = std::make_unique<VariableDeclaration>(
        std::move(name),
        std::move(initializer),
        std::move(typeName)
    );

    declaration->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return declaration;
}


std::unique_ptr<Stmt> Parser::assignmentStatement() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected variable name."
    );

    std::string name = nameToken.lexeme;

    consume(TokenType::Equal, "Expected '=' in assignment.");

    auto value = expression();

    consume(TokenType::Semicolon, "Expected ';' after assignment.");

    auto assignment = std::make_unique<AssignmentStatement>(
        std::move(name),
        std::move(value)
    );

    assignment->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return assignment;
}

std::unique_ptr<Stmt> Parser::incrementStatement() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected variable name."
    );

    std::string name = nameToken.lexeme;

    consume(TokenType::PlusPlus, "Expected '++'.");

    consume(TokenType::Semicolon, "Expected ';' after increment.");

    auto increment = std::make_unique<IncrementStatement>(
        std::move(name)
    );

    increment->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return increment;
}

std::unique_ptr<Stmt> Parser::decrementStatement() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected variable name."
    );

    std::string name = nameToken.lexeme;

    consume(TokenType::MinusMinus, "Expected '--'.");

    consume(TokenType::Semicolon, "Expected ';' after decrement.");

    auto statement = std::make_unique<DecrementStatement>(
        std::move(name)
    );

    statement->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return statement;
}

std::unique_ptr<Stmt> Parser::updateExpression() {
    Token nameToken = consume(
        TokenType::Identifier,
        "Expected variable name in loop update."
    );

    std::string name = nameToken.lexeme;
    std::unique_ptr<Stmt> update;

    if (match(TokenType::PlusPlus)) {
        update = std::make_unique<IncrementStatement>(name);
    } else if (match(TokenType::MinusMinus)) {
        update = std::make_unique<DecrementStatement>(name);
    } else if (match(TokenType::Equal)) {
        auto value = expression();

        update = std::make_unique<AssignmentStatement>(
            name,
            std::move(value)
        );
    } else {
        throw std::runtime_error(
            "Expected ++, --, or '=' in loop update."
        );
    }

    update->location = {
        static_cast<std::size_t>(nameToken.line),
        static_cast<std::size_t>(nameToken.column)
    };

    return update;
}
std::unique_ptr<Stmt> Parser::coutStatement() {
    Token coutToken = previous();

    consume(TokenType::LeftParen, "Expected '(' after cout.");

    auto value = expression();

    consume(TokenType::RightParen, "Expected ')' after cout expression.");
    consume(TokenType::Semicolon, "Expected ';' after cout.");

    auto statement = std::make_unique<CoutStatement>(std::move(value));

    statement->location = {
        static_cast<std::size_t>(coutToken.line),
        static_cast<std::size_t>(coutToken.column)
    };

    return statement;
}

std::unique_ptr<BlockStatement> Parser::blockStatement() {
    Token braceToken = previous();

    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        statements.push_back(statement());
    }

    consume(TokenType::RightBrace, "Expected '}' after block.");

    auto block = std::make_unique<BlockStatement>(
        std::move(statements)
    );

    block->location = {
        static_cast<std::size_t>(braceToken.line),
        static_cast<std::size_t>(braceToken.column)
    };

    return block;
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    Token ifToken = previous();

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

    auto statement = std::make_unique<IfStatement>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch)
    );

    statement->location = {
        static_cast<std::size_t>(ifToken.line),
        static_cast<std::size_t>(ifToken.column)
    };

    return statement;
}

std::unique_ptr<Stmt> Parser::forStatement() {
    Token forToken = previous();

    consume(TokenType::LeftParen, "Expected '(' after for.");
    consume(TokenType::Let, "Expected 'let' in for loop.");

    Token variableToken = consume(
        TokenType::Identifier,
        "Expected loop variable."
    );

    std::string variable = variableToken.lexeme;

    consume(TokenType::Equal, "Expected '=' after loop variable.");

    auto start = expression();

    if (match(TokenType::Comma)) {
        bool multipleInitializers =
            check(TokenType::Identifier) &&
            current + 1 < tokens.size() &&
            tokens[current + 1].type == TokenType::Equal;

        if (!multipleInitializers) {
            auto range = rangeForStatement(
                std::move(variable),
                std::move(start)
            );

            range->location = {
                static_cast<std::size_t>(forToken.line),
                static_cast<std::size_t>(forToken.column)
            };

            return range;
        }
    }

    std::vector<std::unique_ptr<Stmt>> initializers;

    auto firstInitializer = std::make_unique<VariableDeclaration>(
        variable,
        std::move(start)
    );

    firstInitializer->location = {
        static_cast<std::size_t>(variableToken.line),
        static_cast<std::size_t>(variableToken.column)
    };

    initializers.push_back(std::move(firstInitializer));

    while (previous().type == TokenType::Comma) {
        Token nameToken = consume(
            TokenType::Identifier,
            "Expected variable name in loop initializer."
        );

        std::string name = nameToken.lexeme;

        consume(TokenType::Equal, "Expected '=' after variable name.");

        auto value = expression();

        auto initializer = std::make_unique<VariableDeclaration>(
            name,
            std::move(value)
        );

        initializer->location = {
            static_cast<std::size_t>(nameToken.line),
            static_cast<std::size_t>(nameToken.column)
        };

        initializers.push_back(std::move(initializer));

        if (!match(TokenType::Comma)) {
            break;
        }
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

    auto statement = std::make_unique<ForStatement>(
        std::move(initializers),
        std::move(condition),
        std::move(increments),
        std::move(body)
    );

    statement->location = {
        static_cast<std::size_t>(forToken.line),
        static_cast<std::size_t>(forToken.column)
    };

    return statement;
}

std::unique_ptr<Stmt> Parser::rangeForStatement(
    std::string variable,
    std::unique_ptr<Expr> start
) {
    SourceLocation location = start->location;

    bool inclusive = match(TokenType::Equal);

    auto end = expression();

    std::unique_ptr<Expr> step = nullptr;

    if (match(TokenType::Comma)) {
        step = expression();
    }

    consume(TokenType::RightParen, "Expected ')' after range.");
    consume(TokenType::LeftBrace, "Expected '{' before loop body.");

    auto body = blockStatement();

    auto statement = std::make_unique<RangeForStatement>(
        std::move(variable),
        std::move(start),
        std::move(end),
        std::move(step),
        inclusive,
        std::move(body)
    );

    statement->location = location;

    return statement;
}

std::unique_ptr<Expr> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<Expr> Parser::logicalOr() {
    auto expr = logicalAnd();

    while (match(TokenType::OrOr)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = logicalAnd();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    auto expr = equality();

    while (match(TokenType::AndAnd)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = equality();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
    }

    return expr;
}


std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = comparison();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
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
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = term();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
    }

    return expr;
}
std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = factor();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match(TokenType::Star) || match(TokenType::Slash)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = unary();

        auto binary = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );

        binary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        expr = std::move(binary);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match(TokenType::Minus) || match(TokenType::Plus)) {
        Token opToken = previous();
        std::string op = opToken.lexeme;
        auto right = unary();

        auto unary = std::make_unique<UnaryExpr>(
            op,
            std::move(right)
        );

        unary->location = {
            static_cast<std::size_t>(opToken.line),
            static_cast<std::size_t>(opToken.column)
        };

        return unary;
    }

    return primary();
}



std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::True)) {
        auto expr = std::make_unique<BooleanExpr>(true);
        expr->location = {
            static_cast<std::size_t>(previous().line),
            static_cast<std::size_t>(previous().column)
        };
        return expr;
    }

    if (match(TokenType::False)) {
        auto expr = std::make_unique<BooleanExpr>(false);
        expr->location = {
            static_cast<std::size_t>(previous().line),
            static_cast<std::size_t>(previous().column)
        };
        return expr;
    }

    if (match(TokenType::Number)) {
        auto expr = std::make_unique<NumberExpr>(
            std::stoll(previous().lexeme)
        );
        expr->location = {
            static_cast<std::size_t>(previous().line),
            static_cast<std::size_t>(previous().column)
        };
        return expr;
    }

    if (match(TokenType::Identifier)) {
        Token token = previous();
        std::string name = token.lexeme;

        if (match(TokenType::LeftParen)) {
            std::vector<std::unique_ptr<Expr>> arguments;

            if (!check(TokenType::RightParen)) {
                do {
                    arguments.push_back(expression());
                } while (match(TokenType::Comma));
            }

            consume(
                TokenType::RightParen,
                "Expected ')' after function arguments."
            );

            auto expr = std::make_unique<CallExpr>(
                std::move(name),
                std::move(arguments)
            );

            expr->location = {
                static_cast<std::size_t>(token.line),
                static_cast<std::size_t>(token.column)
            };

            return expr;
        }

        auto expr = std::make_unique<VariableExpr>(std::move(name));
        expr->location = {
            static_cast<std::size_t>(token.line),
            static_cast<std::size_t>(token.column)
        };

        return expr;
    }

    if (match(TokenType::LeftParen)) {
        auto expr = expression();

        consume(TokenType::RightParen, "Expected ')' after expression.");

        return expr;
    }

    throw std::runtime_error("Expected expression.");
}


}
