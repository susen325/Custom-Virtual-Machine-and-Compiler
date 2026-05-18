#ifndef VM_HPP
#define VM_HPP

#include "compiler.hpp"
#include <vector>

namespace cvm {
    class VM {
    public:
        VM();
        void interpret(const Chunk& chunk);

    private:
        const Chunk* currentChunk;
        size_t ip = 0;
        std::vector<Value> stack;
        std::vector<Value> locals;

        void push(Value value);
        Value pop();
        uint8_t readByte();
        Value readConstant();

        void binaryOp(OpCode op);
    };
}

#endif // VM_HPP
