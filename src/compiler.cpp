#include "compiler.hpp"

namespace cvm {
    void Chunk::disassemble(const std::string& name) {
        std::cout << "== " << name << " ==" << std::endl;
        for (size_t offset = 0; offset < code.size(); ) {
            uint8_t instruction = code[offset];
            std::cout << offset << " ";
            switch (static_cast<OpCode>(instruction)) {
                case OpCode::CONSTANT: {
                    uint8_t constant = code[offset + 1];
                    std::cout << "CONSTANT " << (int)constant << " '";
                    Value val = constants[constant];
                    if (std::holds_alternative<int>(val)) std::cout << std::get<int>(val);
                    else std::cout << (std::get<bool>(val) ? "true" : "false");
                    std::cout << "'" << std::endl;
                    offset += 2;
                    break;
                }
                case OpCode::ADD:      std::cout << "ADD" << std::endl;      offset += 1; break;
                case OpCode::SUBTRACT: std::cout << "SUBTRACT" << std::endl; offset += 1; break;
                case OpCode::MULTIPLY: std::cout << "MULTIPLY" << std::endl; offset += 1; break;
                case OpCode::DIVIDE:   std::cout << "DIVIDE" << std::endl;   offset += 1; break;
                case OpCode::EQUAL:    std::cout << "EQUAL" << std::endl;    offset += 1; break;
                case OpCode::LESS:     std::cout << "LESS" << std::endl;     offset += 1; break;
                case OpCode::GREATER:  std::cout << "GREATER" << std::endl;  offset += 1; break;
                case OpCode::AND:      std::cout << "AND" << std::endl;      offset += 1; break;
                case OpCode::OR:       std::cout << "OR" << std::endl;       offset += 1; break;
                case OpCode::PRINT:    std::cout << "PRINT" << std::endl;    offset += 1; break;
                case OpCode::GET_LOCAL: std::cout << "GET_LOCAL " << (int)code[offset + 1] << std::endl; offset += 2; break;
                case OpCode::SET_LOCAL: std::cout << "SET_LOCAL " << (int)code[offset + 1] << std::endl; offset += 2; break;
                case OpCode::JUMP: {
                    uint16_t jump = (code[offset + 1] << 8) | code[offset + 2];
                    std::cout << "JUMP " << jump << " -> " << offset + 3 + jump << std::endl;
                    offset += 3;
                    break;
                }
                case OpCode::JUMP_IF_FALSE: {
                    uint16_t jump = (code[offset + 1] << 8) | code[offset + 2];
                    std::cout << "JUMP_IF_FALSE " << jump << " -> " << offset + 3 + jump << std::endl;
                    offset += 3;
                    break;
                }
                case OpCode::LOOP: {
                    uint16_t jump = (code[offset + 1] << 8) | code[offset + 2];
                    std::cout << "LOOP " << jump << " -> " << offset + 3 - jump << std::endl;
                    offset += 3;
                    break;
                }
                case OpCode::INPUT:    std::cout << "INPUT" << std::endl;    offset += 1; break;
                case OpCode::HALT:     std::cout << "HALT" << std::endl;     offset += 1; break;
                default: std::cout << "Unknown opcode " << (int)instruction << std::endl; offset += 1; break;
            }
        }
    }

    Compiler::Compiler() {}

    Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
        chunk = Chunk();
        for (const auto& stmt : statements) {
            compileStmt(stmt.get());
        }
        emitByte(static_cast<uint8_t>(OpCode::HALT));
        return chunk;
    }

    void Compiler::compileStmt(const Stmt* stmt) {
        if (auto s = dynamic_cast<const VarStmt*>(stmt)) {
            if (s->initializer) {
                compileExpr(s->initializer.get());
            } else {
                emitConstant(0);
            }
            int index = locals.size();
            locals[s->name.lexeme] = index;
            emitBytes(static_cast<uint8_t>(OpCode::SET_LOCAL), static_cast<uint8_t>(index));
        } else if (auto s = dynamic_cast<const IfStmt*>(stmt)) {
            compileExpr(s->condition.get());
            int thenJump = emitJump(static_cast<uint8_t>(OpCode::JUMP_IF_FALSE));
            
            compileStmt(s->thenBranch.get());
            
            int elseJump = emitJump(static_cast<uint8_t>(OpCode::JUMP));
            
            patchJump(thenJump);
            
            if (s->elseBranch) {
                compileStmt(s->elseBranch.get());
            }
            patchJump(elseJump);
        } else if (auto s = dynamic_cast<const WhileStmt*>(stmt)) {
            int loopStart = chunk.code.size();
            compileExpr(s->condition.get());
            
            int exitJump = emitJump(static_cast<uint8_t>(OpCode::JUMP_IF_FALSE));
            compileStmt(s->body.get());
            
            emitLoop(loopStart);
            patchJump(exitJump);
        } else if (auto s = dynamic_cast<const BlockStmt*>(stmt)) {
            for (const auto& statement : s->statements) {
                compileStmt(statement.get());
            }
        } else if (auto s = dynamic_cast<const PrintStmt*>(stmt)) {
            compileExpr(s->expression.get());
            emitByte(static_cast<uint8_t>(OpCode::PRINT));
        } else if (auto s = dynamic_cast<const ExpressionStmt*>(stmt)) {
            compileExpr(s->expression.get());
        }
    }

    void Compiler::compileExpr(const Expr* expr) {
        if (auto e = dynamic_cast<const LiteralExpr*>(expr)) {
            emitConstant(e->value);
        } else if (auto e = dynamic_cast<const InputExpr*>(expr)) {
            emitByte(static_cast<uint8_t>(OpCode::INPUT));
        } else if (auto e = dynamic_cast<const AssignExpr*>(expr)) {
            compileExpr(e->value.get());
            if (locals.count(e->name.lexeme)) {
                emitBytes(static_cast<uint8_t>(OpCode::SET_LOCAL), locals[e->name.lexeme]);
            } else {
                std::cerr << "Undefined variable for assignment: " << e->name.lexeme << std::endl;
            }
        } else if (auto e = dynamic_cast<const BinaryExpr*>(expr)) {
            compileExpr(e->left.get());
            compileExpr(e->right.get());
            switch (e->op.type) {
                case TokenType::PLUS:  emitByte(static_cast<uint8_t>(OpCode::ADD)); break;
                case TokenType::MINUS: emitByte(static_cast<uint8_t>(OpCode::SUBTRACT)); break;
                case TokenType::STAR:  emitByte(static_cast<uint8_t>(OpCode::MULTIPLY)); break;
                case TokenType::SLASH: emitByte(static_cast<uint8_t>(OpCode::DIVIDE)); break;
                case TokenType::EQUAL_EQUAL: emitByte(static_cast<uint8_t>(OpCode::EQUAL)); break;
                case TokenType::LESS:  emitByte(static_cast<uint8_t>(OpCode::LESS)); break;
                case TokenType::GREATER: emitByte(static_cast<uint8_t>(OpCode::GREATER)); break;
                case TokenType::AND:   emitByte(static_cast<uint8_t>(OpCode::AND)); break;
                case TokenType::OR:    emitByte(static_cast<uint8_t>(OpCode::OR)); break;
                default: break;
            }
        } else if (auto e = dynamic_cast<const VariableExpr*>(expr)) {
            if (locals.count(e->name.lexeme)) {
                emitBytes(static_cast<uint8_t>(OpCode::GET_LOCAL), locals[e->name.lexeme]);
            } else {
                std::cerr << "Undefined variable: " << e->name.lexeme << std::endl;
            }
        } else if (auto e = dynamic_cast<const UnaryExpr*>(expr)) {
            compileExpr(e->right.get());
            if (e->op.type == TokenType::MINUS) {
                emitConstant(-1);
                emitByte(static_cast<uint8_t>(OpCode::MULTIPLY));
            }
        }
    }

    void Compiler::emitByte(uint8_t byte) {
        chunk.write(byte);
    }

    void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
        emitByte(byte1);
        emitByte(byte2);
    }

    void Compiler::emitConstant(Value value) {
        int index = chunk.addConstant(value);
        emitBytes(static_cast<uint8_t>(OpCode::CONSTANT), static_cast<uint8_t>(index));
    }

    int Compiler::emitJump(uint8_t instruction) {
        emitByte(instruction);
        emitByte(0xff);
        emitByte(0xff);
        return chunk.code.size() - 2;
    }

    void Compiler::patchJump(int offset) {
        int jump = chunk.code.size() - offset - 2;
        chunk.code[offset] = (jump >> 8) & 0xff;
        chunk.code[offset + 1] = jump & 0xff;
    }

    void Compiler::emitLoop(int loopStart) {
        emitByte(static_cast<uint8_t>(OpCode::LOOP));
        int offset = chunk.code.size() - loopStart + 2;
        emitByte((offset >> 8) & 0xff);
        emitByte(offset & 0xff);
    }
}
