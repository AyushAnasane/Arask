
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace newlang {

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    std::int64_t value;

    explicit NumberExpr(std::int64_t value)
        : value(value) {}
};

struct VariableExpr : Expr {
    std::string name;

    explicit VariableExpr(std::string name)
        : name(std::move(name)) {}
};



struct CallExpr : Expr {
    std::string name;
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(
        std::string name,
        std::vector<std::unique_ptr<Expr>> arguments
    )
        : name(std::move(name)),
          arguments(std::move(arguments)) {}
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

struct ReturnStatement : Stmt {
    std::unique_ptr<Expr> value;

    explicit ReturnStatement(std::unique_ptr<Expr> value)
        : value(std::move(value)) {}
};


struct VariableDeclaration : Stmt {
    std::string name;
    std::unique_ptr<Expr> initializer;
    std::optional<std::string> typeName;

    VariableDeclaration(
        std::string name,
        std::unique_ptr<Expr> initializer,
        std::optional<std::string> typeName = std::nullopt
    )
        : name(std::move(name)),
          initializer(std::move(initializer)),
          typeName(std::move(typeName)) {}
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

struct DecrementStatement : Stmt {
    std::string name;

    explicit DecrementStatement(std::string name)
        : name(std::move(name)) {}
};

struct CoutStatement : Stmt {
    std::unique_ptr<Expr> expression;

    explicit CoutStatement(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
};

struct FunctionCallStatement : Stmt {
    std::string name;
    std::vector<std::unique_ptr<Expr>> arguments;

    FunctionCallStatement(
        std::string name,
        std::vector<std::unique_ptr<Expr>> arguments
    )
        : name(std::move(name)),
          arguments(std::move(arguments)) {}
};

struct BlockStatement : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
};

// A parameter keeps the spelling of its declared type for the later type-checking
// phase. Runtime parameter values are introduced when function calls are added.
struct Parameter {
    std::string name;
    std::string typeName;

    Parameter(std::string name, std::string typeName)
        : name(std::move(name)),
          typeName(std::move(typeName)) {}
};

struct FunctionDeclaration : Stmt {
    std::string name;
    std::vector<Parameter> parameters;
    std::optional<std::string> returnType;
    std::unique_ptr<BlockStatement> body;

    FunctionDeclaration(
        std::string name,
        std::vector<Parameter> parameters,
        std::optional<std::string> returnType,
        std::unique_ptr<BlockStatement> body
    )
        : name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)) {}
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
    std::vector<std::unique_ptr<Stmt>> initializers;
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> increments;
    std::unique_ptr<Stmt> body;

    ForStatement(
        std::vector<std::unique_ptr<Stmt>> initializers,
        std::unique_ptr<Expr> condition,
        std::vector<std::unique_ptr<Stmt>> increments,
        std::unique_ptr<Stmt> body
    )
        : initializers(std::move(initializers)),
          condition(std::move(condition)),
          increments(std::move(increments)),
          body(std::move(body)) {}
};

struct RangeForStatement : Stmt {
    std::string variable;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> end;
    std::unique_ptr<Expr> step;
    bool inclusive;
    std::unique_ptr<Stmt> body;

    RangeForStatement(
        std::string variable,
        std::unique_ptr<Expr> start,
        std::unique_ptr<Expr> end,
        std::unique_ptr<Expr> step,
        bool inclusive,
        std::unique_ptr<Stmt> body
    )
        : variable(std::move(variable)),
          start(std::move(start)),
          end(std::move(end)),
          step(std::move(step)),
          inclusive(inclusive),
          body(std::move(body)) {}
};

}
