
#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace newlang {

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    int value;

    explicit NumberExpr(int value)
        : value(value) {}
};

struct VariableExpr : Expr {
    std::string name;

    explicit VariableExpr(std::string name)
        : name(std::move(name)) {}
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;

    BinaryExpr(
        std::unique_ptr<Expr> left,
        std::string op,
        std::unique_ptr<Expr> right
    )
        : left(std::move(left)),
          op(std::move(op)),
          right(std::move(right)) {}
};

struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> right;

    UnaryExpr(
        std::string op,
        std::unique_ptr<Expr> right
    )
        : op(std::move(op)),
          right(std::move(right)) {}
};

struct Stmt {
    virtual ~Stmt() = default;
};

struct VariableDeclaration : Stmt {
    std::string name;
    std::unique_ptr<Expr> initializer;

    VariableDeclaration(
        std::string name,
        std::unique_ptr<Expr> initializer
    )
        : name(std::move(name)),
          initializer(std::move(initializer)) {}
};

struct AssignmentStatement : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignmentStatement(
        std::string name,
        std::unique_ptr<Expr> value
    )
        : name(std::move(name)),
          value(std::move(value)) {}
};

struct IncrementStatement : Stmt {
    std::string name;

    explicit IncrementStatement(std::string name)
        : name(std::move(name)) {}
};

struct CoutStatement : Stmt {
    std::unique_ptr<Expr> expression;

    explicit CoutStatement(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
};

struct BlockStatement : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
};

struct IfStatement : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStatement(
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Stmt> thenBranch,
        std::unique_ptr<Stmt> elseBranch
    )
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
};

struct ForStatement : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> increment;
    std::unique_ptr<Stmt> body;

    ForStatement(
        std::unique_ptr<Stmt> initializer,
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Stmt> increment,
        std::unique_ptr<Stmt> body
    )
        : initializer(std::move(initializer)),
          condition(std::move(condition)),
          increment(std::move(increment)),
          body(std::move(body)) {}
};

}
