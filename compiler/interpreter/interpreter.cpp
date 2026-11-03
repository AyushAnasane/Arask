#include "interpreter/interpreter.hpp"
#include <iostream>
#include <stdexcept>

namespace newlang {

int Interpreter::evaluate(const Expr& expression) {

    if (const auto* number = dynamic_cast<const NumberExpr*>(&expression)) {
        return number->value;
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        auto it = environment.find(variable->name);

        if (it == environment.end()) {
            throw std::runtime_error("Undefined variable: " + variable->name);
        }

        return it->second;
    }


    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        int value = evaluate(*unary->right);

        if (unary->op == "-") return -value;
        if (unary->op == "+") return value;
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {

        int left = evaluate(*binary->left);
        int right = evaluate(*binary->right);

        if (binary->op == "+") return left + right;
        if (binary->op == "-") return left - right;
        if (binary->op == "*") return left * right;
        if (binary->op == "/") {
            if (right == 0) {
                throw std::runtime_error("Division by zero.");
            }

            return left / right;
        }
        if (binary->op == "==") return left == right;
        if (binary->op == "!=") return left != right;
        if (binary->op == "<") return left < right;
        if (binary->op == "<=") return left <= right;
        if (binary->op == ">") return left > right;
        if (binary->op == ">=") return left >= right;
    }

    throw std::runtime_error("Unsupported expression.");
}

void Interpreter::execute(const Stmt& statement) {

    if (const auto* declaration =
        dynamic_cast<const VariableDeclaration*>(&statement)) {

        int value = evaluate(*declaration->initializer);

        environment[declaration->name] = value;

        return;
    }

    if (const auto* assignment = dynamic_cast<const AssignmentStatement*>(&statement)) {
        if (environment.find(assignment->name) == environment.end()) {
            throw std::runtime_error("Undefined variable: " + assignment->name);
        }

        int value = evaluate(*assignment->value);
        environment[assignment->name] = value;
        return;
    }

    if (const auto* increment = dynamic_cast<const IncrementStatement*>(&statement)) {
        auto it = environment.find(increment->name);

        if (it == environment.end()) {
            throw std::runtime_error("Undefined variable: " + increment->name);
        }

        it->second++;
        return;
    }

    if (const auto* output = dynamic_cast<const CoutStatement*>(&statement)) {
        int value = evaluate(*output->expression);

        std::cout<< value << '\n';

        return;
    }

    if (const auto* block = dynamic_cast<const BlockStatement*>(&statement)) {
        for (const auto& child : block->statements) {
            execute(*child);
        }
        return;
    }

    if (const auto* ifStmt = dynamic_cast<const IfStatement*>(&statement)) {
        int condition = evaluate(*ifStmt->condition);

        if (condition != 0) {
            execute(*ifStmt->thenBranch);
        }
        else if (ifStmt->elseBranch) {
            execute(*ifStmt->elseBranch);
        }
        return;
    }

    if (const auto* forStmt = dynamic_cast<const ForStatement*>(&statement)) {
    if (forStmt->initializer) {
        execute(*forStmt->initializer);
    }

    while (evaluate(*forStmt->condition) != 0) {
        execute(*forStmt->body);
        execute(*forStmt->increment);
    }

    return;
}

    throw std::runtime_error("Unsupported statement.");
}


void Interpreter::execute(
    const std::vector<std::unique_ptr<Stmt>>& statements
) {
    for (const auto& statement : statements) {
        execute(*statement);
    }
}

}