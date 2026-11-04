
#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

#include "ast/ast.hpp"
#include "runtime/value.hpp"

namespace newlang {

struct Variable {
    RuntimeValue value;
    std::string typeName;
};

class Interpreter {
private:
    std::vector<std::unordered_map<std::string, Variable>> environments;
    std::unordered_map<std::string, const FunctionDeclaration*> functions;

    std::optional<RuntimeValue> executeFunction(
        const FunctionDeclaration& function,
        const std::vector<RuntimeValue>& arguments
    );

public:
    Interpreter();

    RuntimeValue evaluate(const Expr& expression);
    std::optional<RuntimeValue> execute(const Stmt& statement);
    void execute(const std::vector<std::unique_ptr<Stmt>>& statements);
};

}
