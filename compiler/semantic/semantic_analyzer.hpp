
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/ast.hpp"

namespace newlang {

class SemanticAnalyzer {
private:
    using Scope = std::unordered_map<std::string, std::string>;

    std::vector<Scope> scopes;
    std::unordered_map<std::string, const FunctionDeclaration*> functions;

    void analyzeFunction(const FunctionDeclaration& function);
    void analyzeStatement(const Stmt& statement);
    std::string analyzeExpression(const Expr& expression);

    void beginScope();
    void endScope();

public:
    SemanticAnalyzer();

    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);
};

}
