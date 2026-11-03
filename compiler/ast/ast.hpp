#pragma once

#include <memory>
#include <string>
#include <utility>

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

struct CoutStatement : Stmt {
    std::unique_ptr<Expr> expression;

    explicit CoutStatement(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
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

}