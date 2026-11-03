#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "ast/ast.hpp"

using namespace newlang;

int main() {
    std::string source =
        "let total = 10 + 20;\n"
        "cout(total);";

    Lexer lexer(source);
    auto tokens = lexer.scanTokens();

    Parser parser(tokens);
    auto statements = parser.parse();

    assert(statements.size() == 2);

    auto* declaration =
        dynamic_cast<VariableDeclaration*>(statements[0].get());

    assert(declaration != nullptr);
    assert(declaration->name == "total");

    auto* binary =
        dynamic_cast<BinaryExpr*>(declaration->initializer.get());

    assert(binary != nullptr);
    assert(binary->op == "+");

    auto* left = dynamic_cast<NumberExpr*>(binary->left.get());
    auto* right = dynamic_cast<NumberExpr*>(binary->right.get());

    assert(left != nullptr);
    assert(right != nullptr);

    assert(left->value == 10);
    assert(right->value == 20);

    auto* output =
        dynamic_cast<CoutStatement*>(statements[1].get());

    assert(output != nullptr);

    auto* variable =
        dynamic_cast<VariableExpr*>(output->expression.get());

    assert(variable != nullptr);
    assert(variable->name == "total");

    std::string unarySource = "let x = -10;";

    Lexer unaryLexer(unarySource);
    auto unaryTokens = unaryLexer.scanTokens();

    Parser unaryParser(unaryTokens);
    auto unaryStatements = unaryParser.parse();

    assert(unaryStatements.size() == 1);

    auto* unaryDeclaration =
        dynamic_cast<VariableDeclaration*>(unaryStatements[0].get());

    assert(unaryDeclaration != nullptr);
    assert(unaryDeclaration->name == "x");

    auto* unary =
        dynamic_cast<UnaryExpr*>(unaryDeclaration->initializer.get());

    assert(unary != nullptr);
    assert(unary->op == "-");

    auto* operand =
        dynamic_cast<NumberExpr*>(unary->right.get());

    assert(operand != nullptr);
    assert(operand->value == 10);

    std::string plusSource = "let y = +5;";

    Lexer plusLexer(plusSource);
    auto plusTokens = plusLexer.scanTokens();

    Parser plusParser(plusTokens);
    auto plusStatements = plusParser.parse();

    assert(plusStatements.size() == 1);

    auto* plusDeclaration =
        dynamic_cast<VariableDeclaration*>(plusStatements[0].get());

    assert(plusDeclaration != nullptr);
    assert(plusDeclaration->name == "y");

    auto* plus =
        dynamic_cast<UnaryExpr*>(plusDeclaration->initializer.get());

    assert(plus != nullptr);
    assert(plus->op == "+");

    auto* plusOperand =
        dynamic_cast<NumberExpr*>(plus->right.get());

    assert(plusOperand != nullptr);
    assert(plusOperand->value == 5);

    std::cout << "All parser tests passed!\n";

    return 0;
}