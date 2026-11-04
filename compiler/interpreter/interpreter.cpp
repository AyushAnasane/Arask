#include "interpreter/interpreter.hpp"
#include <iostream>
#include <stdexcept>
#include <limits>
#include <vector>
#include <cstdint>
#include <type_traits>


namespace newlang {

    namespace {

        bool checkedAdd(std::int64_t a, std::int64_t b, std::int64_t& result) {
            if (b > 0 && a > std::numeric_limits<std::int64_t>::max() - b)
                return false;

            if (b < 0 && a < std::numeric_limits<std::int64_t>::min() - b)
                return false;

            result = a + b;
            return true;
        }

        bool checkedSubtract(std::int64_t a, std::int64_t b, std::int64_t& result) {
            if (b < 0 && a > std::numeric_limits<std::int64_t>::max() + b)
                return false;

            if (b > 0 && a < std::numeric_limits<std::int64_t>::min() + b)
                return false;

            result = a - b;
            return true;
        }

        bool checkedMultiply(std::int64_t a, std::int64_t b, std::int64_t& result) {
            if (a == 0 || b == 0) {
                result = 0;
                return true;
            }

            if (a == -1 && b == std::numeric_limits<std::int64_t>::min())
                return false;

            if (b == -1 && a == std::numeric_limits<std::int64_t>::min())
                return false;

            if (a > 0) {
                if (b > 0 && a > std::numeric_limits<std::int64_t>::max() / b)
                    return false;

                if (b < 0 && b < std::numeric_limits<std::int64_t>::min() / a)
                    return false;
            } else {
                if (b > 0 && a < std::numeric_limits<std::int64_t>::min() / b)
                    return false;

                if (b < 0 && a < std::numeric_limits<std::int64_t>::max() / b)
                    return false;
            }

            result = a * b;
            return true;
        }


        std::string inferType(const RuntimeValue& value) {
            if (std::holds_alternative<std::int32_t>(value)) {
                return "i32";
            }

            return "i64";
        }

        std::string normalizeType(const std::string& typeName) {
            if (typeName == "int") {
                return "i32";
            }

            if (typeName == "i32" || typeName == "i64") {
                return typeName;
            }

            throw std::runtime_error("Unsupported integer type: " + typeName);
        }

        Variable createVariable(
            RuntimeValue value,
            const std::optional<std::string>& declaredType = std::nullopt
        ) {
            std::string typeName = declaredType
                ? normalizeType(*declaredType)
                : inferType(value);

            if (typeName == "i32") {
                std::int64_t number = std::visit(
                    [](auto value) {
                        return static_cast<std::int64_t>(value);
                    },
                    value
                );

                if (number < std::numeric_limits<std::int32_t>::min() ||
                    number > std::numeric_limits<std::int32_t>::max()) {
                    throw std::runtime_error("32-bit integer overflow.");
                }

                value = RuntimeValue{static_cast<std::int32_t>(number)};
            } else {
                value = std::visit(
                    [](auto number) -> RuntimeValue {
                        return RuntimeValue{static_cast<std::int64_t>(number)};
                    },
                    value
                );
            }

            return Variable{value, typeName};
        }

        void assignVariable(Variable& variable, RuntimeValue value) {
            variable = createVariable(value, variable.typeName);
        }


}

    Interpreter::Interpreter() {
        environments.emplace_back();
    }


RuntimeValue Interpreter::evaluate(const Expr& expression) {

    if (const auto* number = dynamic_cast<const NumberExpr*>(&expression)) {
        if (number->value >= std::numeric_limits<std::int32_t>::min() &&
            number->value <= std::numeric_limits<std::int32_t>::max()) {
            return RuntimeValue{static_cast<std::int32_t>(number->value)};
        }

        return RuntimeValue{number->value};
    }

    if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error("Undefined function: " + call->name);
        }

        std::vector<RuntimeValue> arguments;

        for (const auto& argument : call->arguments) {
            arguments.push_back(evaluate(*argument));
        }

        std::optional<RuntimeValue> result = executeFunction(
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
                return value->second.value;
            }
        }

        throw std::runtime_error("Undefined variable: " + variable->name);
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
    RuntimeValue value = evaluate(*unary->right);

    if (unary->op == "+") {
        return value;
    }

    if (unary->op == "-") {
        return std::visit([](auto number) -> RuntimeValue {
            using T = decltype(number);

            if (number == std::numeric_limits<T>::min()) {
                throw std::runtime_error("Integer overflow.");
            }

            return RuntimeValue{static_cast<T>(-number)};
        }, value);
    }
}

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
    RuntimeValue leftValue = evaluate(*binary->left);
    RuntimeValue rightValue = evaluate(*binary->right);

    std::int64_t left = std::visit(
        [](auto value) { return static_cast<std::int64_t>(value); },
        leftValue
    );

    std::int64_t right = std::visit(
        [](auto value) { return static_cast<std::int64_t>(value); },
        rightValue
    );

    bool use64 =
        std::holds_alternative<std::int64_t>(leftValue) ||
        std::holds_alternative<std::int64_t>(rightValue);

    if (binary->op == "==") return RuntimeValue{static_cast<std::int32_t>(left == right)};
    if (binary->op == "!=") return RuntimeValue{static_cast<std::int32_t>(left != right)};
    if (binary->op == "<") return RuntimeValue{static_cast<std::int32_t>(left < right)};
    if (binary->op == "<=") return RuntimeValue{static_cast<std::int32_t>(left <= right)};
    if (binary->op == ">") return RuntimeValue{static_cast<std::int32_t>(left > right)};
    if (binary->op == ">=") return RuntimeValue{static_cast<std::int32_t>(left >= right)};

    std::int64_t result = 0;
    bool success = false;

    if (binary->op == "+") {
        success = checkedAdd(left, right, result);
    } else if (binary->op == "-") {
        success = checkedSubtract(left, right, result);
    } else if (binary->op == "*") {
        success = checkedMultiply(left, right, result);
    } else if (binary->op == "/") {
        if (right == 0)
            throw std::runtime_error("Division by zero.");

        if (left == std::numeric_limits<std::int64_t>::min() && right == -1)
            throw std::runtime_error("Integer overflow.");

        result = left / right;
        success = true;
    }

    if (!success)
        throw std::runtime_error("Integer overflow.");

    if (!use64) {
        if (result < std::numeric_limits<std::int32_t>::min() ||
            result > std::numeric_limits<std::int32_t>::max()) {
            throw std::runtime_error("32-bit integer overflow.");
        }

        return RuntimeValue{static_cast<std::int32_t>(result)};
    }

    return RuntimeValue{result};
}
throw std::runtime_error("Unsupported expression.");
}




std::optional<RuntimeValue> Interpreter::executeFunction(
    const FunctionDeclaration& function,
    const std::vector<RuntimeValue>& arguments
) {
    if (arguments.size() != function.parameters.size()) {
        throw std::runtime_error(
            "Wrong number of arguments for function: " + function.name
        );
    }

    environments.emplace_back();

    try {
        for (std::size_t i = 0; i < arguments.size(); ++i) {
            environments.back()[function.parameters[i].name] =
                createVariable(
                    arguments[i],
                    function.parameters[i].typeName
                );
        }

        std::optional<RuntimeValue> result = execute(*function.body);

        if (function.returnType) {
            if (!result.has_value()) {
                throw std::runtime_error(
                    "Function did not return a value: " + function.name
                );
            }

            result = createVariable(
                *result,
                function.returnType
            ).value;
        }

        environments.pop_back();

        return result;
    }
    catch (...) {
        environments.pop_back();
        throw;
    }
}





std::optional<RuntimeValue> Interpreter::execute(const Stmt& statement) {

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

        std::vector<RuntimeValue> arguments;

        for (const auto& argument : call->arguments) {
            arguments.push_back(evaluate(*argument));
        }

        executeFunction(*function->second, arguments);
        return std::nullopt;
    }

    if (const auto* declaration =
        dynamic_cast<const VariableDeclaration*>(&statement)) {

        RuntimeValue value = evaluate(*declaration->initializer);

        environments.back()[declaration->name] = createVariable(
            value,
            declaration->typeName
        );

        return std::nullopt;
    }

    if (const auto* assignment =
        dynamic_cast<const AssignmentStatement*>(&statement)) {

        for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
            auto variable = it->find(assignment->name);

            if (variable != it->end()) {
                assignVariable(
                    variable->second,
                    evaluate(*assignment->value)
                );
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
                std::visit([](auto& value) {
                    using T = std::decay_t<decltype(value)>;

                    if (value == std::numeric_limits<T>::max()) {
                        throw std::runtime_error("Integer overflow.");
                    }

                    ++value;
                }, variable->second.value);

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
                std::visit([](auto& value) {
                    using T = std::decay_t<decltype(value)>;

                    if (value == std::numeric_limits<T>::min()) {
                        throw std::runtime_error("Integer overflow.");
                    }

                    --value;
                }, variable->second.value);

                return std::nullopt;
            }
        }

        throw std::runtime_error("Undefined variable: " + decrement->name);
    }

    if (const auto* output = dynamic_cast<const CoutStatement*>(&statement)) {
        auto value = evaluate(*output->expression);

        std::visit([](const auto& number) {
            std::cout << number << '\n';
        }, value);

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
        auto condition = evaluate(*ifStmt->condition);

        if (std::visit([](auto value) { return value != 0; }, condition)) {
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

        while (
                std::visit(
                    [](auto value) { return value != 0; },
                    evaluate(*forStmt->condition)
                )
            ) {
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

    RuntimeValue startValue = evaluate(*range->start);
    RuntimeValue endValue = evaluate(*range->end);

    RuntimeValue stepValue = range->step
        ? evaluate(*range->step)
        : RuntimeValue{std::int32_t{1}};

    auto toInt64 = [](const RuntimeValue& value) {
        return std::visit(
            [](auto number) {
                return static_cast<std::int64_t>(number);
            },
            value
        );
    };

    std::int64_t start = toInt64(startValue);
    std::int64_t end = toInt64(endValue);
    std::int64_t step = toInt64(stepValue);

    bool use64 =
        std::holds_alternative<std::int64_t>(startValue) ||
        std::holds_alternative<std::int64_t>(endValue) ||
        std::holds_alternative<std::int64_t>(stepValue);

    if (step <= 0) {
        throw std::runtime_error(
            "Range step must be a positive integer."
        );
    }

    int direction = (start <= end) ? 1 : -1;
    std::int64_t current = start;

    while (
        direction == 1
            ? (range->inclusive ? current <= end : current < end)
            : (range->inclusive ? current >= end : current > end)
    ) {
        if (use64) {
            environments.back()[range->variable] =
                createVariable(RuntimeValue{current});
        } else {
            environments.back()[range->variable] =
                createVariable(RuntimeValue{static_cast<std::int32_t>(current)});
        }

        auto result = execute(*range->body);

        if (result.has_value()) {
            return result;
        }

        std::int64_t next = 0;

        bool success = direction == 1
            ? checkedAdd(current, step, next)
            : checkedSubtract(current, step, next);

        if (!success) {
            break;
        }

        if (!use64 &&
            (next < std::numeric_limits<std::int32_t>::min() ||
             next > std::numeric_limits<std::int32_t>::max())) {
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
