
#include "semantic/semantic_analyzer.hpp"

#include <stdexcept>
#include <utility>
#include <limits>

namespace newlang {

    namespace {

        [[noreturn]] void throwSemanticError(
            const SourceLocation& location,
            const std::string& message
        ) {
            throw std::runtime_error(
                "Semantic error at line " +
                std::to_string(location.line) +
                ", column " +
                std::to_string(location.column) +
                ": " +
                message
            );
        }

        }

SemanticAnalyzer::SemanticAnalyzer() = default;

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
    currentFunctionName.clear();
    currentReturnType.reset();

    ScopeGuard globalScope(*this);

    for (const auto& statement : statements) {
        if (const auto* function =
            dynamic_cast<const FunctionDeclaration*>(statement.get())) {

            if (!functions.emplace(function->name, function).second) {
                throwSemanticError(
                    statement->location,
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
    ScopeGuard functionScope(*this);

    std::string previousFunctionName = currentFunctionName;
    std::optional<std::string> previousReturnType = currentReturnType;

    try {
        currentFunctionName = function.name;
        currentReturnType = function.returnType;

        if (currentReturnType && *currentReturnType == "int") {
            currentReturnType = "i32";
        }

        if (currentReturnType &&
            *currentReturnType != "i32" &&
            *currentReturnType != "i64" &&
            *currentReturnType != "bool") {
            throwSemanticError(
                function.location,
                "Unsupported return type '" + *currentReturnType +
                "' in function '" + currentFunctionName + "'."
            );
        }

        for (const auto& parameter : function.parameters) {
            std::string type = parameter.typeName;

            if (type == "int") {
                type = "i32";
            }

            if (type != "i32" && type != "i64" && type != "bool") {
                throwSemanticError(
                    parameter.location,
                    "Unsupported parameter type '" + type +
                    "' in function '" + function.name + "'."
                );
            }

            if (!scopes.back().emplace(parameter.name, type).second) {
                throwSemanticError(
                    parameter.location,
                    "Duplicate parameter: " + parameter.name
                );
            }
        }

        analyzeStatement(*function.body);

        if (currentReturnType && !alwaysReturns(*function.body)) {
            throwSemanticError(
                function.location,
                "Not all control-flow paths return a value in function '" +
                currentFunctionName + "'."
            );
        }
    } catch (...) {
        currentFunctionName.swap(previousFunctionName);
        currentReturnType.swap(previousReturnType);
        throw;
    }

    currentFunctionName.swap(previousFunctionName);
    currentReturnType.swap(previousReturnType);
}





std::string SemanticAnalyzer::analyzeExpression(
    const Expr& expression
) {
    if (dynamic_cast<const BooleanExpr*>(&expression)) {
        return "bool";
    }

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

        throwSemanticError(
            expression.location,
            "Undefined variable: " + variable->name
        );
    }

    if (const auto* unary =
        dynamic_cast<const UnaryExpr*>(&expression)) {

        std::string type = analyzeExpression(*unary->right);

        if (type != "i32" && type != "i64") {
            throwSemanticError(
                expression.location,
                "Invalid unary operand type."
            );
        }

        if (unary->op != "+" && unary->op != "-") {
            throwSemanticError(
                expression.location,
                "Unsupported unary operator: " + unary->op
            );
        }

        return type;
    }

    if (const auto* binary =
        dynamic_cast<const BinaryExpr*>(&expression)) {

        std::string left = analyzeExpression(*binary->left);
        std::string right = analyzeExpression(*binary->right);

        if (left != right) {
            throwSemanticError(
                expression.location,
                "Type mismatch in binary expression."
            );
        }

        const std::string& op = binary->op;

        if (op == "==" || op == "!=") {
            if (left != "i32" && left != "i64" && left != "bool") {
                throwSemanticError(
                    expression.location,
                    "Invalid equality operand type."
                );
            }

            return "bool";
        }

        if (op == "<" || op == "<=" || op == ">" || op == ">=") {
            if (left != "i32" && left != "i64") {
                throwSemanticError(
                    expression.location,
                    "Ordering comparisons require numeric operands."
                );
            }

            return "bool";
        }

        if (op == "+" || op == "-" || op == "*" || op == "/") {
            if (left != "i32" && left != "i64") {
                throwSemanticError(
                    expression.location,
                    "Arithmetic operators require numeric operands."
                );
            }

            return left;
        }

        throwSemanticError(
            expression.location,
            "Unsupported binary operator: " + op
        );
    }

    if (const auto* call =
        dynamic_cast<const CallExpr*>(&expression)) {

        auto function = functions.find(call->name);

        if (function == functions.end()) {
            throwSemanticError(
                expression.location,
                "Undefined function: " + call->name
            );
        }

        const auto& declaration = *function->second;

        if (call->arguments.size() != declaration.parameters.size()) {
            throwSemanticError(
                expression.location,
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
                throwSemanticError(
                    call->arguments[i]->location,
                    "Type mismatch in argument " + std::to_string(i + 1) +
                    " of function '" + call->name + "'."
                );
            }
        }

        std::string returnType = declaration.returnType.value_or("void");

        if (returnType == "int") {
            returnType = "i32";
        }

        return returnType;
    }

    throwSemanticError(
        expression.location,
        "Unsupported expression in semantic analysis."
    );
}






void SemanticAnalyzer::analyzeStatement(const Stmt& statement) {
    if (const auto* declaration =
        dynamic_cast<const VariableDeclaration*>(&statement)) {

        std::string actual = analyzeExpression(*declaration->initializer);
        std::string type = declaration->typeName.value_or(actual);

        if (type == "int") {
            type = "i32";
        }

        if (type != "i32" && type != "i64" && type != "bool") {
            throwSemanticError(
                statement.location,
                "Unsupported variable type: " + type
            );
        }

        if (actual != type) {
            throwSemanticError(
                declaration->initializer->location,
                "Type mismatch in variable declaration: " +
                declaration->name
            );
        }

        if (!scopes.back().emplace(declaration->name, type).second) {
            throwSemanticError(
                statement.location,
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
                    throwSemanticError(
                        assignment->value->location,
                        "Type mismatch in assignment: " + assignment->name
                    );
                }

                return;
            }
        }

        throwSemanticError(
            statement.location,
            "Undefined variable: " + assignment->name
        );
    }

    if (const auto* increment =
        dynamic_cast<const IncrementStatement*>(&statement)) {

        VariableExpr variable(increment->name);
        variable.location = statement.location;

        std::string type = analyzeExpression(variable);

        if (type != "i32" && type != "i64") {
            throwSemanticError(
                statement.location,
                "Invalid increment operand type."
            );
        }

        return;
    }

    if (const auto* decrement =
        dynamic_cast<const DecrementStatement*>(&statement)) {

        VariableExpr variable(decrement->name);
        variable.location = statement.location;

        std::string type = analyzeExpression(variable);

        if (type != "i32" && type != "i64") {
            throwSemanticError(
                statement.location,
                "Invalid decrement operand type."
            );
        }

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
            throwSemanticError(
                statement.location,
                "Undefined function: " + call->name
            );
        }

        if (call->arguments.size() != function->second->parameters.size()) {
            throwSemanticError(
                statement.location,
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
                throwSemanticError(
                    call->arguments[i]->location,
                    "Type mismatch in function call: " + call->name
                );
            }
        }

        return;
    }

    if (const auto* block =
        dynamic_cast<const BlockStatement*>(&statement)) {

        ScopeGuard scope(*this);

        for (const auto& child : block->statements) {
            analyzeStatement(*child);
        }

        return;
    }

    if (const auto* ifStatement =
        dynamic_cast<const IfStatement*>(&statement)) {

        std::string condition =
            analyzeExpression(*ifStatement->condition);

        if (condition != "bool" &&
            condition != "i32" &&
            condition != "i64") {
            throwSemanticError(
                ifStatement->condition->location,
                "Invalid if condition."
            );
        }

        analyzeStatement(*ifStatement->thenBranch);

        if (ifStatement->elseBranch) {
            analyzeStatement(*ifStatement->elseBranch);
        }

        return;
    }

    if (const auto* returnStatement =
        dynamic_cast<const ReturnStatement*>(&statement)) {

        if (!returnStatement->value) {
            if (currentReturnType) {
                throwSemanticError(
                    statement.location,
                    "Function '" + currentFunctionName +
                    "' must return a value of type " + *currentReturnType + "."
                );
            }

            return;
        }

        if (!currentReturnType) {
            throwSemanticError(
                statement.location,
                "Function '" + currentFunctionName +
                "' cannot return a value because it has no declared return type."
            );
        }

        std::string actual = analyzeExpression(*returnStatement->value);

        if (actual != *currentReturnType) {
            throwSemanticError(
                returnStatement->value->location,
                "Return type mismatch in function '" + currentFunctionName +
                "': expected " + *currentReturnType +
                ", got " + actual + "."
            );
        }

        return;
    }

    if (const auto* forStatement =
        dynamic_cast<const ForStatement*>(&statement)) {

        ScopeGuard loopScope(*this);

        for (const auto& initializer : forStatement->initializers) {
            analyzeStatement(*initializer);
        }

        std::string condition =
            analyzeExpression(*forStatement->condition);

        if (condition != "bool" &&
            condition != "i32" &&
            condition != "i64") {
            throwSemanticError(
                forStatement->condition->location,
                "Invalid for loop condition."
            );
        }

        for (const auto& increment : forStatement->increments) {
            analyzeStatement(*increment);
        }

        analyzeStatement(*forStatement->body);
        return;
    }

    if (const auto* range =
        dynamic_cast<const RangeForStatement*>(&statement)) {

        ScopeGuard rangeScope(*this);

        std::string start = analyzeExpression(*range->start);
        std::string end = analyzeExpression(*range->end);

        if (start != "i32" && start != "i64") {
            throwSemanticError(
                range->start->location,
                "Range boundaries must be integers."
            );
        }

        if (start != end) {
            throwSemanticError(
                range->end->location,
                "Range type mismatch."
            );
        }

        if (!scopes.back().emplace(range->variable, start).second) {
            throwSemanticError(
                statement.location,
                "Duplicate range variable: " + range->variable
            );
        }

        if (range->step) {
            std::string step = analyzeExpression(*range->step);

            if (step != "i32" && step != "i64") {
                throwSemanticError(
                    range->step->location,
                    "Range step must be an integer."
                );
            }

            if (step != start) {
                throwSemanticError(
                    range->step->location,
                    "Range step type mismatch."
                );
            }
        }

        analyzeStatement(*range->body);
        return;
    }

    throwSemanticError(
        statement.location,
        "Unsupported statement in semantic analysis."
    );
}



bool SemanticAnalyzer::alwaysReturns(const Stmt& statement) {
    if (dynamic_cast<const ReturnStatement*>(&statement)) {
        return true;
    }

    if (const auto* block =
        dynamic_cast<const BlockStatement*>(&statement)) {

        for (const auto& child : block->statements) {
            if (alwaysReturns(*child)) {
                return true;
            }
        }

        return false;
    }

    if (const auto* ifStatement =
        dynamic_cast<const IfStatement*>(&statement)) {

        if (!ifStatement->elseBranch) {
            return false;
        }

        return alwaysReturns(*ifStatement->thenBranch) &&
               alwaysReturns(*ifStatement->elseBranch);
    }

    return false;
}
}
