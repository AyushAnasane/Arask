
#pragma once

#include <memory>
#include <string>

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

// Represents an operation, e.g. age + 5
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

// Represents: print(age + 5);
struct PrintStatement : Stmt {
    std::unique_ptr<Expr> expression;

    explicit PrintStatement(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
};

}
