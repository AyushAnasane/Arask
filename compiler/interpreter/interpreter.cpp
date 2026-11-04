#include "interpreter/interpreter.hpp"
#include <iostream>
#include <stdexcept>
#include <limits>
#include <vector>


namespace newlang {

    Interpreter::Interpreter() {
        environments.emplace_back();
    }

int Interpreter::evaluate(const Expr& expression) {

    if (const auto* number = dynamic_cast<const NumberExpr*>(&expression)) {
        return number->value;
    }


    if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error("Undefined function: " + call->name);
        }

        std::vector<int> arguments;

        for (const auto& argument : call->arguments) {
            arguments.push_back(evaluate(*argument));
        }

        std::optional<int> result = executeFunction(
            *function->second,
            arguments
        );

        if (!result.has_value()) {
            throw std::runtime_error(
                "Function did not return a value: " + call->name
            );
        }

        return *result;
    }


    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
            auto value = it->find(variable->name);

            if (value != it->end()) {
                return value->second;
            }
        }

        throw std::runtime_error("Undefined variable: " + variable->name);
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


std::optional<int> Interpreter::executeFunction(
    const FunctionDeclaration& function,
    const std::vector<int>& arguments
) {
    if (arguments.size() != function.parameters.size()) {
        throw std::runtime_error(
            "Wrong number of arguments for function: " + function.name
        );
    }

    environments.emplace_back();

    try {
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            environments.back()[function.parameters[i].name] = arguments[i];
        }

        std::optional<int> result = execute(*function.body);

        environments.pop_back();

        return result;
    }
    catch (...) {
        environments.pop_back();
        throw;
    }
}



std::optional<int> Interpreter::execute(const Stmt& statement) {

    if (dynamic_cast<const FunctionDeclaration*>(&statement)) {
        return std::nullopt;
    }

    if (const auto* returnStmt =
        dynamic_cast<const ReturnStatement*>(&statement)) {
        return evaluate(*returnStmt->value);
    }

    if (const auto* call = dynamic_cast<const FunctionCallStatement*>(&statement)) {
        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error("Undefined function: " + call->name);
        }

        std::vector<int> arguments;

        for (const auto& argument : call->arguments) {
            arguments.push_back(evaluate(*argument));
        }

        executeFunction(*function->second, arguments);
        return std::nullopt;
    }

    if (const auto* declaration =
        dynamic_cast<const VariableDeclaration*>(&statement)) {

        int value = evaluate(*declaration->initializer);
        environments.back()[declaration->name] = value;

        return std::nullopt;
    }

    if (const auto* assignment =
        dynamic_cast<const AssignmentStatement*>(&statement)) {

        for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
            auto variable = it->find(assignment->name);

            if (variable != it->end()) {
                int value = evaluate(*assignment->value);
                variable->second = value;
                return std::nullopt;
            }
        }

        throw std::runtime_error("Undefined variable: " + assignment->name);
    }

    if (const auto* increment =
        dynamic_cast<const IncrementStatement*>(&statement)) {

        for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
            auto variable = it->find(increment->name);

            if (variable != it->end()) {
                variable->second++;
                return std::nullopt;
            }
        }

        throw std::runtime_error("Undefined variable: " + increment->name);
    }

    if (const auto* decrement =
        dynamic_cast<const DecrementStatement*>(&statement)) {

        for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
            auto variable = it->find(decrement->name);

            if (variable != it->end()) {
                variable->second--;
                return std::nullopt;
            }
        }

        throw std::runtime_error("Undefined variable: " + decrement->name);
    }

    if (const auto* output = dynamic_cast<const CoutStatement*>(&statement)) {
        int value = evaluate(*output->expression);
        std::cout << value << '\n';

        return std::nullopt;
    }

    if (const auto* block = dynamic_cast<const BlockStatement*>(&statement)) {
        for (const auto& child : block->statements) {
            auto result = execute(*child);

            if (result.has_value()) {
                return result;
            }
        }

        return std::nullopt;
    }

    if (const auto* ifStmt = dynamic_cast<const IfStatement*>(&statement)) {
        int condition = evaluate(*ifStmt->condition);

        if (condition != 0) {
            return execute(*ifStmt->thenBranch);
        }

        if (ifStmt->elseBranch) {
            return execute(*ifStmt->elseBranch);
        }

        return std::nullopt;
    }

    if (const auto* forStmt = dynamic_cast<const ForStatement*>(&statement)) {

        for (const auto& initializer : forStmt->initializers) {
            auto result = execute(*initializer);

            if (result.has_value()) {
                return result;
            }
        }

        while (evaluate(*forStmt->condition) != 0) {
            auto result = execute(*forStmt->body);

            if (result.has_value()) {
                return result;
            }

            for (const auto& increment : forStmt->increments) {
                result = execute(*increment);

                if (result.has_value()) {
                    return result;
                }
            }
        }

        return std::nullopt;
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
            environments.back()[range->variable] = static_cast<int>(current);

            auto result = execute(*range->body);

            if (result.has_value()) {
                return result;
            }

            long long next = current +
                (direction * static_cast<long long>(step));

            if (next < std::numeric_limits<int>::min() ||
                next > std::numeric_limits<int>::max()) {
                break;
            }

            current = next;
        }

        return std::nullopt;
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

   
    if (mainFunction) {
        executeFunction(*mainFunction, {});
        return;
    }

    for (const auto& statement : statements) {
        execute(*statement);
    }
}

}
