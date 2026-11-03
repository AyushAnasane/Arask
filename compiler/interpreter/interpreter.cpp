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

    if (const auto* output = dynamic_cast<const CoutStatement*>(&statement)) {
        int value = evaluate(*output->expression);

        std::cout<< value << '\n';

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