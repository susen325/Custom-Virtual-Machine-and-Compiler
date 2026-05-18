#include "vm.hpp"
#include <iostream>

namespace cvm {
    VM::VM() {
        locals.resize(256);
    }

    void VM::interpret(const Chunk& chunk) {
        currentChunk = &chunk;
        ip = 0;

        for (;;) {
            uint8_t instruction = readByte();
            switch (static_cast<OpCode>(instruction)) {
                case OpCode::CONSTANT: {
                    push(readConstant());
                    break;
                }
                case OpCode::AND:      binaryOp(OpCode::AND); break;
                case OpCode::OR:      binaryOp(OpCode::OR); break;
                case OpCode::ADD:      binaryOp(OpCode::ADD); break;
                case OpCode::SUBTRACT: binaryOp(OpCode::SUBTRACT); break;
                case OpCode::MULTIPLY: binaryOp(OpCode::MULTIPLY); break;
                case OpCode::DIVIDE:   binaryOp(OpCode::DIVIDE); break;
                case OpCode::EQUAL:    binaryOp(OpCode::EQUAL); break;
                case OpCode::LESS:     binaryOp(OpCode::LESS); break;
                case OpCode::GREATER:  binaryOp(OpCode::GREATER); break;
                case OpCode::PRINT: {
                    Value value = pop();
                    if (std::holds_alternative<int>(value)) {
                        std::cout << std::get<int>(value) << std::endl;
                    } else {
                        std::cout << (std::get<bool>(value) ? "true" : "false") << std::endl;
                    }
                    break;
                }
                case OpCode::GET_LOCAL: {
                    uint8_t slot = readByte();
                    push(locals[slot]);
                    break;
                }
                case OpCode::SET_LOCAL: {
                    uint8_t slot = readByte();
                    locals[slot] = pop();
                    break;
                }
                case OpCode::JUMP: {
                    uint16_t offset = (readByte() << 8) | readByte();
                    ip += offset;
                    break;
                }
                case OpCode::JUMP_IF_FALSE: {
                    uint16_t offset = (readByte() << 8) | readByte();
                    Value condition = pop();
                    if (std::holds_alternative<bool>(condition) && !std::get<bool>(condition)) {
                        ip += offset;
                    }
                    break;
                }
                case OpCode::LOOP: {
                    uint16_t offset = (readByte() << 8) | readByte();
                    ip -= offset;
                    break;
                }
                case OpCode::INPUT: {
                    int val;
                    std::cout << "? ";
                    std::cin >> val;
                    push(val);
                    break;
                }
                case OpCode::HALT:
                    return;
            }
        }
    }

    uint8_t VM::readByte() {
        return currentChunk->code[ip++];
    }

    Value VM::readConstant() {
        return currentChunk->constants[readByte()];
    }

    void VM::push(Value value) {
        stack.push_back(value);
    }

    Value VM::pop() {
        Value value = stack.back();
        stack.pop_back();
        return value;
    }

    void VM::binaryOp(OpCode op) {
        Value b = pop();
        Value a = pop();

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            int va = std::get<int>(a);
            int vb = std::get<int>(b);
            switch (op) {
                case OpCode::ADD:      push(va + vb); break;
                case OpCode::SUBTRACT: push(va - vb); break;
                case OpCode::MULTIPLY: push(va * vb); break;
                case OpCode::DIVIDE:   push(va / vb); break;
                case OpCode::EQUAL:    push(va == vb); break;
                case OpCode::LESS:     push(va < vb); break;
                case OpCode::GREATER:  push(va > vb); break;
                case OpCode::AND:      push(va && vb); break;
                case OpCode::OR:       push(va || vb); break;
                default: break;
            }
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
             bool va = std::get<bool>(a);
             bool vb = std::get<bool>(b);
             switch (op) {
                case OpCode::EQUAL:    push(va == vb); break;
                case OpCode::AND:      push(va && vb); break;
                case OpCode::OR:       push(va || vb); break;
                default: break;
            }
        }
    }
}
