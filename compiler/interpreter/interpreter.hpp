#pragma once
#include <vector>

#include "ast/ast.hpp"

#include <unordered_map>
#include <string>

namespace newlang {

class Interpreter {
private:
    std::unordered_map<std::string, int> environment;

public:
    int evaluate(const Expr& expression);
    void execute(const Stmt& statement);
    void execute(const std::vector<std::unique_ptr<Stmt>>& statements);
};

}