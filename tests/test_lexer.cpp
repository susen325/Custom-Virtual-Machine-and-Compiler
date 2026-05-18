#include "../src/lexer.hpp"
#include <iostream>
#include <cassert>

void testBasicTokenization() {
    std::string source = "let x = 10; print x + 5;";
    cvm::Lexer lexer(source);
    auto tokens = lexer.tokenize();

    assert(tokens.size() == 11); // let, x, =, 10, ;, print, x, +, 5, ;, EOF
    assert(tokens[0].type == cvm::TokenType::LET);
    assert(tokens[1].type == cvm::TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "x");
    assert(tokens[2].type == cvm::TokenType::EQUAL);
    assert(tokens[3].type == cvm::TokenType::NUMBER);
    assert(std::get<int>(tokens[3].value) == 10);
    assert(tokens[4].type == cvm::TokenType::SEMICOLON);
    assert(tokens[5].type == cvm::TokenType::PRINT);
    assert(tokens[6].type == cvm::TokenType::IDENTIFIER);
    assert(tokens[7].type == cvm::TokenType::PLUS);
    assert(tokens[8].type == cvm::TokenType::NUMBER);
    assert(std::get<int>(tokens[8].value) == 5);
    assert(tokens[9].type == cvm::TokenType::SEMICOLON);
    assert(tokens[10].type == cvm::TokenType::END_OF_FILE);

    std::cout << "Lexer test passed!" << std::endl;
}

int main() {
    testBasicTokenization();
    return 0;
}
