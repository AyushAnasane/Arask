
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "ast/ast.hpp"

namespace newlang {

class SemanticAnalyzer {
private:
    using Scope = std::unordered_map<std::string, std::string>;

    class ScopeGuard {
    private:
        SemanticAnalyzer& analyzer;

    public:
        explicit ScopeGuard(SemanticAnalyzer& analyzer)
            : analyzer(analyzer) {
            analyzer.beginScope();
        }

        ~ScopeGuard() {
            analyzer.endScope();
        }

        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
    };

    std::vector<Scope> scopes;
    std::unordered_map<std::string, const FunctionDeclaration*> functions;

    void analyzeFunction(const FunctionDeclaration& function);
    void analyzeStatement(const Stmt& statement);
    bool alwaysReturns(const Stmt& statement);
    std::string analyzeExpression(const Expr& expression);
    std::optional<std::string> currentReturnType;
    std::string currentFunctionName;

    void beginScope();
    void endScope();

public:
    SemanticAnalyzer();

    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);
};

}
