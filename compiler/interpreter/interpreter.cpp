#include "interpreter/interpreter.hpp"
#include <iostream>
#include <stdexcept>
#include <limits>

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

    if (dynamic_cast<const FunctionDeclaration*>(&statement)) {
        // Function invocation is not implemented yet. A declaration only
        // introduces program structure, so its body must not run at top level.
        return;
    }

    if (const auto* call = dynamic_cast<const FunctionCallStatement*>(&statement)) {
        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error("Undefined function: " + call->name);
        }

        if (call->arguments.size() != function->second->parameters.size()) {
            throw std::runtime_error("Wrong number of arguments for function: " + call->name);
        }

        for (std::size_t i = 0; i < call->arguments.size(); ++i) {
            const std::string& parameterName =
                function->second->parameters[i].name;

            environment[parameterName] = evaluate(*call->arguments[i]);
        }

        execute(*function->second->body);
        return;
    }

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

    if (const auto* decrement = dynamic_cast<const DecrementStatement*>(&statement)) {
        environment[decrement->name]--;
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
    for (const auto& initializer : forStmt->initializers) {
        execute(*initializer);
    }

    while (evaluate(*forStmt->condition) != 0) {
        execute(*forStmt->body);

        for (const auto& increment : forStmt->increments) {
            execute(*increment);
        }
    }

    return;
}

if (const auto* range =
    dynamic_cast<const RangeForStatement*>(&statement)) {

    int start = evaluate(*range->start);
    int end = evaluate(*range->end);

    int step = range->step
        ? evaluate(*range->step)
        : 1;

    if (step <= 0) {
        throw std::runtime_error(
            "Range step must be a positive integer."
        );
    }

    int direction = (start <= end) ? 1 : -1;

    long long current = start;

    while (
        direction == 1
            ? (range->inclusive ? current <= end : current < end)
            : (range->inclusive ? current >= end : current > end)
    ) {
        environment[range->variable] = static_cast<int>(current);

        execute(*range->body);

        long long next = current + (direction * static_cast<long long>(step));

        if (next < std::numeric_limits<int>::min() ||
            next > std::numeric_limits<int>::max()) {
            break;
        }

        current = next;
    }

    return;
}

    throw std::runtime_error("Unsupported statement.");
}


void Interpreter::execute(
    const std::vector<std::unique_ptr<Stmt>>& statements
) {
    functions.clear();
    const FunctionDeclaration* mainFunction = nullptr;

    for (const auto& statement : statements) {
        const auto* function = dynamic_cast<const FunctionDeclaration*>(
            statement.get()
        );

        if (function) {
            if (!functions.emplace(function->name, function).second) {
                throw std::runtime_error(
                    "Program contains more than one function named '" +
                    function->name + "'."
                );
            }

            if (function->name == "main") {
                mainFunction = function;
            }
        }
    }

    // A declared main function is the program's entry point. Until ordinary
    // function calls are implemented, only main receives this special handling.
    if (mainFunction) {
        execute(*mainFunction->body);
        return;
    }

    // Keep earlier script-style programs working when no main function exists.
    for (const auto& statement : statements) {
        execute(*statement);
    }
}

}
