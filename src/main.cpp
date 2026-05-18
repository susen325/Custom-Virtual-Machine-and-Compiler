#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include "vm.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

class Runner {
public:
    void run(const std::string& source, bool showAST, bool showBytecode) {
        try {
            cvm::Lexer lexer(source);
            auto tokens = lexer.tokenize();

            cvm::Parser parser(tokens);
            auto statements = parser.parse();

            if (showAST) {
                std::cout << "--- AST ---" << std::endl;
                for (const auto& stmt : statements) {
                    stmt->print();
                }
                std::cout << "-----------" << std::endl;
            }

            auto chunk = compiler.compile(statements);

            if (showBytecode) {
                chunk.disassemble("Compiled Chunk");
            }

            vm.interpret(chunk);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    cvm::Compiler compiler;
    cvm::VM vm;
};

int main(int argc, char* argv[]) {
    bool showAST = false;
    bool showBytecode = false;
    const char* filename = nullptr;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--ast") == 0) {
            showAST = true;
        } else if (std::strcmp(argv[i], "--code") == 0) {
            showBytecode = true;
        } else {
            filename = argv[i];
        }
    }

    Runner runner;

    if (filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        runner.run(buffer.str(), showAST, showBytecode);
    } else {
        std::cout << "CVM++ REPL (type 'exit' to quit)" << std::endl;
        std::cout << "Flags: --ast (show AST), --code (show Bytecode)" << std::endl;
        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line) || line == "exit") break;
            if (line.empty()) continue;
            runner.run(line, showAST, showBytecode);
        }
    }
    return 0;
}
