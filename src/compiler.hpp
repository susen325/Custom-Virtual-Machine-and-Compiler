#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "parser.hpp"
#include <map>

namespace cvm {
    struct Chunk {
        std::vector<uint8_t> code;
        std::vector<Value> constants;

        void write(uint8_t byte) {
            code.push_back(byte);
        }

        int addConstant(Value value) {
            constants.push_back(value);
            return constants.size() - 1;
        }

        void disassemble(const std::string& name);
    };

    class Compiler {
    public:
        Compiler();
        Chunk compile(const std::vector<std::unique_ptr<Stmt>>& statements);

    private:
        Chunk chunk;
        std::map<std::string, int> locals;

        void compileStmt(const Stmt* stmt);
        void compileExpr(const Expr* expr);

        void emitByte(uint8_t byte);
        void emitBytes(uint8_t byte1, uint8_t byte2);
        void emitConstant(Value value);
        int emitJump(uint8_t instruction);
        void patchJump(int offset);
        void emitLoop(int loopStart);
    };
}

#endif // COMPILER_HPP
