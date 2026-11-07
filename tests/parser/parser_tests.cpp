
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

    std::string precedenceSource = "let result = 10 + 5 * 2;";

    Lexer precedenceLexer(precedenceSource);
    auto precedenceTokens = precedenceLexer.scanTokens();

    Parser precedenceParser(precedenceTokens);
    auto precedenceStatements = precedenceParser.parse();

    assert(precedenceStatements.size() == 1);

    auto* precedenceDeclaration =
        dynamic_cast<VariableDeclaration*>(precedenceStatements[0].get());

    assert(precedenceDeclaration != nullptr);
    assert(precedenceDeclaration->name == "result");

    auto* addition =
        dynamic_cast<BinaryExpr*>(precedenceDeclaration->initializer.get());

    assert(addition != nullptr);
    assert(addition->op == "+");

    auto* firstNumber =
        dynamic_cast<NumberExpr*>(addition->left.get());

    auto* multiplication =
        dynamic_cast<BinaryExpr*>(addition->right.get());

    assert(firstNumber != nullptr);
    assert(firstNumber->value == 10);

    assert(multiplication != nullptr);
    assert(multiplication->op == "*");

    auto* secondNumber =
        dynamic_cast<NumberExpr*>(multiplication->left.get());

    auto* thirdNumber =
        dynamic_cast<NumberExpr*>(multiplication->right.get());

    assert(secondNumber != nullptr);
    assert(thirdNumber != nullptr);

    assert(secondNumber->value == 5);
    assert(thirdNumber->value == 2);

    std::string parenthesesSource = "let result = (10 + 5) * 2;";

    Lexer parenthesesLexer(parenthesesSource);
    auto parenthesesTokens = parenthesesLexer.scanTokens();

    Parser parenthesesParser(parenthesesTokens);
    auto parenthesesStatements = parenthesesParser.parse();

    assert(parenthesesStatements.size() == 1);

    auto* parenthesesDeclaration =
        dynamic_cast<VariableDeclaration*>(parenthesesStatements[0].get());

    assert(parenthesesDeclaration != nullptr);
    assert(parenthesesDeclaration->name == "result");

    auto* parenthesesMultiplication =
        dynamic_cast<BinaryExpr*>(parenthesesDeclaration->initializer.get());

    assert(parenthesesMultiplication != nullptr);
    assert(parenthesesMultiplication->op == "*");

    auto* parenthesesAddition =
        dynamic_cast<BinaryExpr*>(parenthesesMultiplication->left.get());

    assert(parenthesesAddition != nullptr);
    assert(parenthesesAddition->op == "+");

    auto* parenthesesFirst =
        dynamic_cast<NumberExpr*>(parenthesesAddition->left.get());

    auto* parenthesesSecond =
        dynamic_cast<NumberExpr*>(parenthesesAddition->right.get());

    auto* parenthesesThird =
        dynamic_cast<NumberExpr*>(parenthesesMultiplication->right.get());

    assert(parenthesesFirst != nullptr);
    assert(parenthesesSecond != nullptr);
    assert(parenthesesThird != nullptr);

    assert(parenthesesFirst->value == 10);
    assert(parenthesesSecond->value == 5);
    assert(parenthesesThird->value == 2);

    {
    std::string associativitySource = "let result = 20 - 5 - 3;";

    Lexer associativityLexer(associativitySource);
    auto associativityTokens = associativityLexer.scanTokens();

    Parser associativityParser(associativityTokens);
    auto associativityStatements = associativityParser.parse();

    assert(associativityStatements.size() == 1);

    auto* associativityDeclaration =
        dynamic_cast<VariableDeclaration*>(associativityStatements[0].get());

    assert(associativityDeclaration != nullptr);
    assert(associativityDeclaration->name == "result");

    auto* outerSubtraction =
        dynamic_cast<BinaryExpr*>(associativityDeclaration->initializer.get());

    assert(outerSubtraction != nullptr);
    assert(outerSubtraction->op == "-");

    auto* innerSubtraction =
        dynamic_cast<BinaryExpr*>(outerSubtraction->left.get());

    assert(innerSubtraction != nullptr);
    assert(innerSubtraction->op == "-");

    auto* firstNumber =
        dynamic_cast<NumberExpr*>(innerSubtraction->left.get());

    auto* secondNumber =
        dynamic_cast<NumberExpr*>(innerSubtraction->right.get());

    auto* thirdNumber =
        dynamic_cast<NumberExpr*>(outerSubtraction->right.get());

    assert(firstNumber != nullptr);
    assert(secondNumber != nullptr);
    assert(thirdNumber != nullptr);

    assert(firstNumber->value == 20);
    assert(secondNumber->value == 5);
    assert(thirdNumber->value == 3);
    }
    
    {
    std::string comparisonSource = "let result = 10 + 5 <= 15;";

    Lexer comparisonLexer(comparisonSource);
    auto comparisonTokens = comparisonLexer.scanTokens();

    Parser comparisonParser(comparisonTokens);
    auto comparisonStatements = comparisonParser.parse();

    assert(comparisonStatements.size() == 1);

    auto* declaration =
        dynamic_cast<VariableDeclaration*>(comparisonStatements[0].get());

    assert(declaration != nullptr);

    auto* comparison =
        dynamic_cast<BinaryExpr*>(declaration->initializer.get());

    assert(comparison != nullptr);
    assert(comparison->op == "<=");

    auto* addition =
        dynamic_cast<BinaryExpr*>(comparison->left.get());

    assert(addition != nullptr);
    assert(addition->op == "+");
}

{
    auto parseRange = [](const std::string& source) {
        Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        Parser parser(tokens);
        return parser.parse();
    };

    auto getNumber = [](const std::unique_ptr<Expr>& expression) {
        auto* number = dynamic_cast<NumberExpr*>(expression.get());
        assert(number != nullptr);
        return number->value;
    };

    {
        auto statements = parseRange("for (let i = 0, 5) {}");

        assert(statements.size() == 1);

        auto* range = dynamic_cast<RangeForStatement*>(statements[0].get());

        assert(range != nullptr);
        assert(range->variable == "i");
        assert(getNumber(range->start) == 0);
        assert(getNumber(range->end) == 5);
        assert(range->step == nullptr);
        assert(range->inclusive == false);
    }

    {
        auto statements = parseRange("for (let i = 0, =5) {}");

        auto* range = dynamic_cast<RangeForStatement*>(statements[0].get());

        assert(range != nullptr);
        assert(range->inclusive == true);
        assert(getNumber(range->end) == 5);
    }

    {
        auto statements = parseRange("for (let i = 5, 0) {}");

        auto* range = dynamic_cast<RangeForStatement*>(statements[0].get());

        assert(range != nullptr);
        assert(getNumber(range->start) == 5);
        assert(getNumber(range->end) == 0);
        assert(range->inclusive == false);
    }

    {
        auto statements = parseRange("for (let i = 0, 10, 2) {}");

        auto* range = dynamic_cast<RangeForStatement*>(statements[0].get());

        assert(range != nullptr);
        assert(getNumber(range->start) == 0);
        assert(getNumber(range->end) == 10);
        assert(getNumber(range->step) == 2);
    }

    {
        auto statements = parseRange("for (let i = 10, 0, 2) {}");

        auto* range = dynamic_cast<RangeForStatement*>(statements[0].get());

        assert(range != nullptr);
        assert(getNumber(range->start) == 10);
        assert(getNumber(range->end) == 0);
        assert(getNumber(range->step) == 2);
    }
}

{
    std::string functionSource =
        "fn add(left: i32, right: i32) -> i32 {"
        "cout(left + right);"
        "}"
        "fn main() {}";

    Lexer functionLexer(functionSource);
    auto functionTokens = functionLexer.scanTokens();

    Parser functionParser(functionTokens);
    auto functionStatements = functionParser.parse();

    assert(functionStatements.size() == 2);

    auto* add = dynamic_cast<FunctionDeclaration*>(
        functionStatements[0].get()
    );

    assert(add != nullptr);
    assert(add->name == "add");
    assert(add->parameters.size() == 2);
    assert(add->parameters[0].name == "left");
    assert(add->parameters[0].typeName == "i32");
    assert(add->parameters[1].name == "right");
    assert(add->parameters[1].typeName == "i32");
    assert(add->returnType.has_value());
    assert(*add->returnType == "i32");
    assert(add->body->statements.size() == 1);
    assert(dynamic_cast<CoutStatement*>(add->body->statements[0].get()) != nullptr);

    auto* mainFunction = dynamic_cast<FunctionDeclaration*>(
        functionStatements[1].get()
    );

    assert(mainFunction != nullptr);
    assert(mainFunction->name == "main");
    assert(mainFunction->parameters.empty());
    assert(!mainFunction->returnType.has_value());
    assert(mainFunction->body->statements.empty());
}

{
    std::string callSource =
        "fn greet() {}"
        "fn main() { greet(); }";

    Lexer callLexer(callSource);
    auto callTokens = callLexer.scanTokens();

    Parser callParser(callTokens);
    auto callStatements = callParser.parse();

    auto* mainFunction = dynamic_cast<FunctionDeclaration*>(
        callStatements[1].get()
    );

    assert(mainFunction != nullptr);
    assert(mainFunction->body->statements.size() == 1);

    auto* call = dynamic_cast<FunctionCallStatement*>(
        mainFunction->body->statements[0].get()
    );

    assert(call != nullptr);
    assert(call->name == "greet");
    assert(call->arguments.empty());
}

{
    std::string callSource =
        "fn main() { printScore(42); }";

    Lexer callLexer(callSource);
    auto callTokens = callLexer.scanTokens();

    Parser callParser(callTokens);
    auto callStatements = callParser.parse();

    auto* mainFunction = dynamic_cast<FunctionDeclaration*>(
        callStatements[0].get()
    );

    assert(mainFunction != nullptr);
    assert(mainFunction->body->statements.size() == 1);

    auto* call = dynamic_cast<FunctionCallStatement*>(
        mainFunction->body->statements[0].get()
    );

    assert(call != nullptr);
    assert(call->name == "printScore");
    assert(call->arguments.size() == 1);

    auto* argument = dynamic_cast<NumberExpr*>(call->arguments[0].get());
    assert(argument != nullptr);
    assert(argument->value == 42);
}

{
    std::string logicalAndSource = "let result = true && false;";

    Lexer logicalAndLexer(logicalAndSource);
    auto logicalAndTokens = logicalAndLexer.scanTokens();

    Parser logicalAndParser(logicalAndTokens);
    auto logicalAndStatements = logicalAndParser.parse();

    assert(logicalAndStatements.size() == 1);

    auto* declaration = dynamic_cast<VariableDeclaration*>(
        logicalAndStatements[0].get()
    );

    assert(declaration != nullptr);
    assert(declaration->name == "result");

    auto* logicalAnd = dynamic_cast<BinaryExpr*>(
        declaration->initializer.get()
    );

    assert(logicalAnd != nullptr);
    assert(logicalAnd->op == "&&");

    auto* left = dynamic_cast<BooleanExpr*>(logicalAnd->left.get());
    auto* right = dynamic_cast<BooleanExpr*>(logicalAnd->right.get());

    assert(left != nullptr);
    assert(right != nullptr);

    assert(left->value == true);
    assert(right->value == false);
}

{
    std::string logicalOrSource = "let result = true || false;";

    Lexer logicalOrLexer(logicalOrSource);
    auto logicalOrTokens = logicalOrLexer.scanTokens();

    Parser logicalOrParser(logicalOrTokens);
    auto logicalOrStatements = logicalOrParser.parse();

    assert(logicalOrStatements.size() == 1);

    auto* declaration = dynamic_cast<VariableDeclaration*>(
        logicalOrStatements[0].get()
    );

    assert(declaration != nullptr);
    assert(declaration->name == "result");

    auto* logicalOr = dynamic_cast<BinaryExpr*>(
        declaration->initializer.get()
    );

    assert(logicalOr != nullptr);
    assert(logicalOr->op == "||");

    auto* left = dynamic_cast<BooleanExpr*>(logicalOr->left.get());
    auto* right = dynamic_cast<BooleanExpr*>(logicalOr->right.get());

    assert(left != nullptr);
    assert(right != nullptr);

    assert(left->value == true);
    assert(right->value == false);
}

std::cout << "All parser tests passed!\n";

return 0;
}
