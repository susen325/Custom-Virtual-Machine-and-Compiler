#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include <iostream>
#include <cassert>

void testBasicParsing() {
    std::string source = "let x = 10; print x + 5;";
    cvm::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    cvm::Parser parser(tokens);
    auto statements = parser.parse();

    assert(statements.size() == 2);
    
    // Check first statement: let x = 10;
    auto varStmt = dynamic_cast<cvm::VarStmt*>(statements[0].get());
    assert(varStmt != nullptr);
    assert(varStmt->name.lexeme == "x");
    auto initializer = dynamic_cast<cvm::LiteralExpr*>(varStmt->initializer.get());
    assert(initializer != nullptr);
    assert(std::get<int>(initializer->value) == 10);

    // Check second statement: print x + 5;
    auto printStmt = dynamic_cast<cvm::PrintStmt*>(statements[1].get());
    assert(printStmt != nullptr);
    auto binaryExpr = dynamic_cast<cvm::BinaryExpr*>(printStmt->expression.get());
    assert(binaryExpr != nullptr);
    assert(binaryExpr->op.type == cvm::TokenType::PLUS);

    std::cout << "Parser test passed!" << std::endl;
}

int main() {
    testBasicParsing();
    return 0;
}
