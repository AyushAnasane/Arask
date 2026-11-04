#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "ast/ast.hpp"

namespace newlang {

class Interpreter {
private:
    std::vector<std::unordered_map<std::string, int>> environments;
    std::unordered_map<std::string, const FunctionDeclaration*> functions;

    void executeFunction(
        const FunctionDeclaration& function,
        const std::vector<int>& arguments
    );

public:
    Interpreter();

    int evaluate(const Expr& expression);
    void execute(const Stmt& statement);
    void execute(const std::vector<std::unique_ptr<Stmt>>& statements);
};

}