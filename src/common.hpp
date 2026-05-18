#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>
#include <stdint.h>

namespace cvm {
    // OpCodes for the VM
    enum class OpCode : uint8_t {
        CONSTANT,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        EQUAL,
        LESS,
        GREATER,
        AND,
        OR,
        PRINT,
        GET_LOCAL,
        SET_LOCAL,
        JUMP,
        JUMP_IF_FALSE,
        LOOP,
        INPUT,
        HALT
    };

    // Values supported by the language
    using Value = std::variant<int, bool>;
}

#endif // COMMON_HPP
