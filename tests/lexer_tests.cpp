
#include <iostream>
#include <cassert>

#include "lexer/lexer.hpp"

using namespace std;
using namespace newlang;

int main() {

    string source1 =
        "let age = 19;\n"
        "cout(age);";

    Lexer lexer1(source1);
    auto tokens1 = lexer1.scanTokens();

    assert(tokens1.size() == 11);
    assert(tokens1[0].type == TokenType::Let);
    assert(tokens1[1].type == TokenType::Identifier);
    assert(tokens1[2].type == TokenType::Equal);
    assert(tokens1[3].type == TokenType::Number);
    assert(tokens1[4].type == TokenType::Semicolon);
    assert(tokens1[5].type == TokenType::Cout);
    assert(tokens1[10].type == TokenType::EndOfFile);

    assert(tokens1[0].line == 1);
    assert(tokens1[0].column == 1);
    assert(tokens1[5].line == 2);
    assert(tokens1[5].column == 1);

    string source2 =
        "let total = 10 + 20;\n"
        "if (total >= 30) {\n"
        "    cout(total);\n"
        "}";

    Lexer lexer2(source2);
    auto tokens2 = lexer2.scanTokens();

    assert(tokens2[0].type == TokenType::Let);
    assert(tokens2[1].type == TokenType::Identifier);
    assert(tokens2[2].type == TokenType::Equal);
    assert(tokens2[3].type == TokenType::Number);
    assert(tokens2[4].type == TokenType::Plus);
    assert(tokens2[5].type == TokenType::Number);
    assert(tokens2[6].type == TokenType::Semicolon);

    assert(tokens2[7].type == TokenType::If);
    assert(tokens2[8].type == TokenType::LeftParen);
    assert(tokens2[10].type == TokenType::GreaterEqual);
    assert(tokens2[12].type == TokenType::RightParen);
    assert(tokens2[13].type == TokenType::LeftBrace);
    assert(tokens2[14].type == TokenType::Cout);
    assert(tokens2[19].type == TokenType::RightBrace);
    assert(tokens2.back().type == TokenType::EndOfFile);

    string source3 = "fn add() -> i32 {}";

    Lexer lexer3(source3);
    auto tokens3 = lexer3.scanTokens();

    assert(tokens3[0].type == TokenType::Fn);
    assert(tokens3[4].type == TokenType::Arrow);
    assert(tokens3[5].type == TokenType::Identifier);

    string source4 = "&& || ! != & |";

    Lexer lexer4(source4);
    auto tokens4 = lexer4.scanTokens();

    assert(tokens4.size() == 7);
    assert(tokens4[0].type == TokenType::AndAnd);
    assert(tokens4[1].type == TokenType::OrOr);
    assert(tokens4[2].type == TokenType::Bang);
    assert(tokens4[3].type == TokenType::BangEqual);
    assert(tokens4[4].type == TokenType::Ampersand);
    assert(tokens4[5].type == TokenType::Unknown);
    assert(tokens4[6].type == TokenType::EndOfFile);

    cout << "All lexer tests passed!\n";

    return 0;
}
