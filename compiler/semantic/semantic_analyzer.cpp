
#include "semantic/semantic_analyzer.hpp"

#include <stdexcept>
#include <utility>
#include <limits>

namespace newlang {

SemanticAnalyzer::SemanticAnalyzer() {
    beginScope();
}

void SemanticAnalyzer::beginScope() {
    scopes.emplace_back();
}

void SemanticAnalyzer::endScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

void SemanticAnalyzer::analyze(
    const std::vector<std::unique_ptr<Stmt>>& statements
) {
    scopes.clear();
    functions.clear();

    beginScope();

    for (const auto& statement : statements) {
        if (const auto* function =
            dynamic_cast<const FunctionDeclaration*>(statement.get())) {

            if (!functions.emplace(function->name, function).second) {
                throw std::runtime_error(
                    "Duplicate function: " + function->name
                );
            }
        }
    }

    for (const auto& statement : statements) {
        if (const auto* function =
            dynamic_cast<const FunctionDeclaration*>(statement.get())) {
            analyzeFunction(*function);
        }
    }
}

void SemanticAnalyzer::analyzeFunction(
    const FunctionDeclaration& function
) {
    beginScope();

    for (const auto& parameter : function.parameters) {
        std::string type = parameter.typeName;

        if (type == "int") {
            type = "i32";
        }

        if (type != "i32" && type != "i64") {
            throw std::runtime_error(
                "Unsupported parameter type '" + type +
                "' in function '" + function.name + "'."
            );
        }

        if (!scopes.back().emplace(parameter.name, type).second) {
            throw std::runtime_error(
                "Duplicate parameter: " + parameter.name
            );
        }
    }

    analyzeStatement(*function.body);

    endScope();
}

std::string SemanticAnalyzer::analyzeExpression(
    const Expr& expression
) {
    if (const auto* number =
        dynamic_cast<const NumberExpr*>(&expression)) {

        if (number->value >= std::numeric_limits<std::int32_t>::min() &&
            number->value <= std::numeric_limits<std::int32_t>::max()) {
            return "i32";
        }

        return "i64";
    }

    if (const auto* variable =
        dynamic_cast<const VariableExpr*>(&expression)) {

        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(variable->name);

            if (found != it->end()) {
                return found->second;
            }
        }

        throw std::runtime_error(
            "Undefined variable: " + variable->name
        );
    }

    if (const auto* unary =
        dynamic_cast<const UnaryExpr*>(&expression)) {

        std::string type = analyzeExpression(*unary->right);

        if (type != "i32" && type != "i64") {
            throw std::runtime_error("Invalid unary operand type.");
        }

        return type;
    }

    if (const auto* binary =
        dynamic_cast<const BinaryExpr*>(&expression)) {

        std::string left = analyzeExpression(*binary->left);
        std::string right = analyzeExpression(*binary->right);

        if (left != right) {
            throw std::runtime_error(
                "Type mismatch in binary expression."
            );
        }

        if (
            binary->op == "==" ||
            binary->op == "!=" ||
            binary->op == "<" ||
            binary->op == "<=" ||
            binary->op == ">" ||
            binary->op == ">="
        ) {
            return "bool";
        }

        return left;
    }

    if (const auto* call =
        dynamic_cast<const CallExpr*>(&expression)) {

        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error(
                "Undefined function: " + call->name
            );
        }

        const auto& declaration = *function->second;

        if (call->arguments.size() != declaration.parameters.size()) {
            throw std::runtime_error(
                "Incorrect argument count for function: " + call->name
            );
        }

        for (std::size_t i = 0; i < call->arguments.size(); ++i) {
            std::string actual = analyzeExpression(*call->arguments[i]);
            std::string expected = declaration.parameters[i].typeName;

            if (expected == "int") {
                expected = "i32";
            }

            if (actual != expected) {
                throw std::runtime_error(
                    "Type mismatch in argument " + std::to_string(i + 1) +
                    " of function '" + call->name + "'."
                );
            }
        }

        return declaration.returnType.value_or("void");
    }

    throw std::runtime_error("Unsupported expression in semantic analysis.");
}

void SemanticAnalyzer::analyzeStatement(const Stmt& statement) {
    if (const auto* declaration =
        dynamic_cast<const VariableDeclaration*>(&statement)) {

        std::string actual = analyzeExpression(*declaration->initializer);

        std::string type = declaration->typeName.value_or(actual);

        if (type == "int") {
            type = "i32";
        }

        if (type != "i32" && type != "i64") {
            throw std::runtime_error(
                "Unsupported variable type: " + type
            );
        }

        if (actual != type) {
            throw std::runtime_error(
                "Type mismatch in variable declaration: " +
                declaration->name
            );
        }

        if (!scopes.back().emplace(declaration->name, type).second) {
            throw std::runtime_error(
                "Duplicate variable: " + declaration->name
            );
        }

        return;
    }

    if (const auto* assignment =
        dynamic_cast<const AssignmentStatement*>(&statement)) {

        std::string actual = analyzeExpression(*assignment->value);

        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(assignment->name);

            if (found != it->end()) {
                if (found->second != actual) {
                    throw std::runtime_error(
                        "Type mismatch in assignment: " + assignment->name
                    );
                }

                return;
            }
        }

        throw std::runtime_error(
            "Undefined variable: " + assignment->name
        );
    }

    if (const auto* increment =
        dynamic_cast<const IncrementStatement*>(&statement)) {

        analyzeExpression(VariableExpr(increment->name));
        return;
    }

    if (const auto* decrement =
        dynamic_cast<const DecrementStatement*>(&statement)) {

        analyzeExpression(VariableExpr(decrement->name));
        return;
    }

    if (const auto* output =
        dynamic_cast<const CoutStatement*>(&statement)) {

        analyzeExpression(*output->expression);
        return;
    }

    if (const auto* call =
        dynamic_cast<const FunctionCallStatement*>(&statement)) {

        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throw std::runtime_error(
                "Undefined function: " + call->name
            );
        }

        if (call->arguments.size() != function->second->parameters.size()) {
            throw std::runtime_error(
                "Incorrect argument count for function: " + call->name
            );
        }

        for (std::size_t i = 0; i < call->arguments.size(); ++i) {
            std::string actual = analyzeExpression(*call->arguments[i]);
            std::string expected = function->second->parameters[i].typeName;

            if (expected == "int") {
                expected = "i32";
            }

            if (actual != expected) {
                throw std::runtime_error(
                    "Type mismatch in function call: " + call->name
                );
            }
        }

        return;
    }

    if (const auto* block =
        dynamic_cast<const BlockStatement*>(&statement)) {

        beginScope();

        for (const auto& child : block->statements) {
            analyzeStatement(*child);
        }

        endScope();
        return;
    }

    if (const auto* ifStatement =
        dynamic_cast<const IfStatement*>(&statement)) {

        std::string condition =
            analyzeExpression(*ifStatement->condition);

        if (condition != "bool" &&
            condition != "i32" &&
            condition != "i64") {
            throw std::runtime_error("Invalid if condition.");
        }

        analyzeStatement(*ifStatement->thenBranch);

        if (ifStatement->elseBranch) {
            analyzeStatement(*ifStatement->elseBranch);
        }

        return;
    }

    if (const auto* returnStatement =
        dynamic_cast<const ReturnStatement*>(&statement)) {

        analyzeExpression(*returnStatement->value);
        return;
    }

    if (const auto* forStatement =
        dynamic_cast<const ForStatement*>(&statement)) {

        beginScope();

        for (const auto& initializer : forStatement->initializers) {
            analyzeStatement(*initializer);
        }

        analyzeExpression(*forStatement->condition);

        for (const auto& increment : forStatement->increments) {
            analyzeStatement(*increment);
        }

        analyzeStatement(*forStatement->body);

        endScope();
        return;
    }

    if (const auto* range =
        dynamic_cast<const RangeForStatement*>(&statement)) {

        beginScope();

        std::string start = analyzeExpression(*range->start);
        std::string end = analyzeExpression(*range->end);

        if (start != end) {
            throw std::runtime_error("Range type mismatch.");
        }

        scopes.back()[range->variable] = start;

        if (range->step) {
            std::string step = analyzeExpression(*range->step);

            if (step != start) {
                throw std::runtime_error("Range step type mismatch.");
            }
        }

        analyzeStatement(*range->body);

        endScope();
        return;
    }

    throw std::runtime_error("Unsupported statement in semantic analysis.");
}

}
