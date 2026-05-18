#ifndef LEXER_HPP
#define LEXER_HPP

#include "common.hpp"

namespace cvm {
    enum class TokenType {
        // Single-character tokens
        PLUS, MINUS, STAR, SLASH,
        EQUAL, EQUAL_EQUAL, LESS, GREATER,
        AND, OR,
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        SEMICOLON,

        // Literals
        IDENTIFIER, NUMBER,

        // Keywords
        LET, IF, ELSE, WHILE, PRINT, INPUT,
        TRUE, FALSE,

        // Special
        END_OF_FILE, INVALID
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        Value value;
        int line;
    };

    class Lexer {
    public:
        Lexer(const std::string& source);
        std::vector<Token> tokenize();

    private:
        std::string source;
        size_t start = 0;
        size_t current = 0;
        int line = 1;

        bool isAtEnd() const;
        char advance();
        char peek() const;
        void scanToken(std::vector<Token>& tokens);
        void addToken(TokenType type, std::vector<Token>& tokens);
        void addToken(TokenType type, Value value, std::vector<Token>& tokens);
        bool match(char expected);
        void identifier(std::vector<Token>& tokens);
        void number(std::vector<Token>& tokens);
    };
}

#endif // LEXER_HPP
