
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace newlang;

std::string runProgram(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();

    Parser parser(tokens);
    auto statements = parser.parse();

    Interpreter interpreter;

    std::ostringstream output;
    std::streambuf* original = std::cout.rdbuf(output.rdbuf());

    try {
        interpreter.execute(statements);
    } catch (...) {
        std::cout.rdbuf(original);
        throw;
    }

    std::cout.rdbuf(original);

    return output.str();
}

int main() {

    std::cerr << "Test 1: Basic arithmetic\n";
    {
        std::string output = runProgram(
            "let a = 10;"
            "let b = 20;"
            "let result = a + b;"
            "cout(result);"
        );

        assert(output == "30\n");
    }

    std::cerr << "Test 2: Increment and decrement\n";
    {
        std::string output = runProgram(
            "let i = 0;"
            "i++;"
            "i++;"
            "i--;"
            "cout(i);"
        );

        assert(output == "1\n");
    }

    std::cerr << "Test 3: Basic for loop\n";
    {
        std::string output = runProgram(
            "for (let i = 0; i < 3; i++) {"
            "cout(i);"
            "}"
        );

        assert(output == "0\n1\n2\n");
    }

    std::cerr << "Test 4: Multiple loop variables\n";
    {
        std::string output = runProgram(
            "for (let i = 0, j = 10; i < 3; i++, j--) {"
            "cout(i);"
            "cout(j);"
            "}"
        );

        assert(output == "0\n10\n1\n9\n2\n8\n");
    }

    std::cerr << "Test 5: Multiplication update\n";
    {
        std::string output = runProgram(
            "for (let i = 1; i < 100; i = i * 2) {"
            "cout(i);"
            "}"
        );

        assert(output == "1\n2\n4\n8\n16\n32\n64\n");
    }

    std::cerr << "Test 6: Multiple assignment updates\n";
    {
        std::string output = runProgram(
            "for (let i = 0, j = 100; i < 10; i = i + 2, j = j / 2) {"
            "cout(i);"
            "cout(j);"
            "}"
        );

        assert(output == "0\n100\n2\n50\n4\n25\n6\n12\n8\n6\n");
    }

    std::cerr << "Test 7: Ignore functions without main\n";
    {
        std::string output = runProgram(
            "fn hidden() { cout(99); }"
            "cout(1);"
        );

        assert(output == "1\n");
    }

    std::cerr << "Test 8: Main entry point\n";
    {
        std::string output = runProgram(
            "fn helper() { cout(99); }"
            "fn main() {"
            "cout(10);"
            "for (let i = 0; i < 2; i++) { cout(i); }"
            "}"
            "cout(500);"
        );

        assert(output == "10\n0\n1\n");
    }

    std::cerr << "Test 9: Function calls\n";
    {
        std::string output = runProgram(
            "fn main() {"
            "greet();"
            "cout(1);"
            "}"
            "fn greet() { cout(100); }"
        );

        assert(output == "100\n1\n");
    }

    std::cerr << "Test 10: Function parameters\n";
    {
        std::string output = runProgram(
            "fn printScore(score: i32) {"
            "cout(score);"
            "}"
            "fn main() {"
            "printScore(42);"
            "}"
        );

        assert(output == "42\n");
    }

    {
        std::string output = runProgram(
            "fn first() {"
            "let x = 10;"
            "cout(x);"
            "}"
            "fn second() {"
            "let x = 20;"
            "cout(x);"
            "}"
            "fn main() {"
            "first();"
            "second();"
            "}"
        );

        assert(output == "10\n20\n");
    }

    {
        std::string output = runProgram(
            "fn show(value: i32) {"
            "cout(value);"
            "}"
            "fn main() {"
            "show(10);"
            "show(20);"
            "show(30);"
            "}"
        );

        assert(output == "10\n20\n30\n");
    }


std::cerr << "Test 13: Return stops function execution\n";
    {
        std::string output = runProgram(
            "fn test() -> i32 {"
            "cout(10);"
            "return 50;"
            "cout(20);"
            "}"
            "fn main() {"
            "test();"
            "cout(1);"
            "}"
        );
        
        assert(output == "10\n1\n");
        std::cerr << "Captured output:\n" << output;
        
    }


std::cerr << "Test 14: Function return values\n";
    {
        std::string output = runProgram(
            "fn add(a: i32, b: i32) -> i32 {"
            "return a + b;"
            "}"
            "fn main() {"
            "let result = add(10, 20);"
            "cout(result);"
            "}"
        );

        assert(output == "30\n");
    }


std::cerr << "Test 15: Nested function calls\n";
    {
        std::string output = runProgram(
            "fn add(a: i32, b: i32) -> i32 {"
            "return a + b;"
            "}"
            "fn square(n: i32) -> i32 {"
            "return n * n;"
            "}"
            "fn main() {"
            "let result = square(add(2, 3));"
            "cout(result);"
            "}"
        );

        assert(output == "25\n");
    }



std::cerr << "Test 16: Recursive factorial\n";
    {
        std::string output = runProgram(
            "fn factorial(n: i32) -> i32 {"
            "if (n <= 1) {"
            "return 1;"
            "}"
            "return n * factorial(n - 1);"
            "}"
            "fn main() {"
            "cout(factorial(5));"
            "}"
        );

        assert(output == "120\n");
    }


    {
    std::string output = runProgram(
        "fn zero() -> i32 {"
        "return 0;"
        "}"
        "fn main() {"
        "cout(zero());"
        "}"
    );

    assert(output == "0\n");
}

{
    std::string output = runProgram(
        "fn find() -> i32 {"
        "for (let i = 0; i < 10; i++) {"
        "if (i == 4) {"
        "return i;"
        "}"
        "}"
        "return -1;"
        "}"
        "fn main() {"
        "cout(find());"
        "}"
    );

    assert(output == "4\n");
}


{
    std::string output = runProgram(
        "fn choose(n: i32) -> i32 {"
        "if (n > 10) {"
        "return 100;"
        "}"
        "return 1;"
        "}"
        "fn main() {"
        "cout(choose(20));"
        "cout(choose(5));"
        "}"
    );

    assert(output == "100\n1\n");
}

    std::cerr << "Test 17: Boolean literals\n";
        {
            std::string output = runProgram(
                "fn main() {"
                "cout(true);"
                "cout(false);"
                "}"
            );

            assert(output == "true\nfalse\n");
        }

        std::cerr << "Test 18: Boolean variables and assignment\n";
            {
                std::string output = runProgram(
                    "fn main() {"
                    "let isReady: bool = true;"
                    "cout(isReady);"
                    "isReady = false;"
                    "cout(isReady);"
                    "}"
                );

                assert(output == "true\nfalse\n");
            }

            std::cerr << "Test 19: Boolean conditions\n";
    {
    std::string output = runProgram(
            "fn main() {"
            "let isReady: bool = true;"
            "if (isReady) {"
            "cout(1);"
            "}"
            "isReady = false;"
            "if (isReady) {"
            "cout(2);"
            "}"
            "}"
        );

        assert(output == "1\n");
    }

    std::cerr << "Test 20: Boolean function parameters\n";
    {
        std::string output = runProgram(
            "fn showStatus(status: bool) {"
            "if (status) {"
            "cout(true);"
            "}"
            "}"
            "fn main() {"
            "showStatus(true);"
            "showStatus(false);"
            "}"
        );

        assert(output == "true\n");
    }

    std::cerr << "Test 21: Boolean function return values\n";
        {
            std::string output = runProgram(
                "fn isReady() -> bool {"
                "return true;"
                "}"
                "fn main() {"
                "let result: bool = isReady();"
                "cout(result);"
                "}"
            );

            assert(output == "true\n");
        }

        std::cerr << "Test 22: Boolean equality\n";
            {
                std::string output = runProgram(
                    "fn main() {"
                    "cout(true == false);"
                    "cout(true == true);"
                    "cout(false != true);"
                    "}"
                );

                assert(output == "false\ntrue\ntrue\n");
            }

    std::cerr << "All interpreter tests passed!\n";

    return 0;
}
