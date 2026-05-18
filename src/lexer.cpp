#include "lexer.hpp"
#include <cctype>

namespace cvm {
    Lexer::Lexer(const std::string& source) : source(source) {}

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;
        while (!isAtEnd()) {
            start = current;
            scanToken(tokens);
        }
        tokens.push_back({TokenType::END_OF_FILE, "", 0, line});
        return tokens;
    }

    bool Lexer::isAtEnd() const {
        return current >= source.length();
    }

    char Lexer::advance() {
        return source[current++];
    }

    char Lexer::peek() const {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    bool Lexer::match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    void Lexer::addToken(TokenType type, std::vector<Token>& tokens) {
        addToken(type, 0, tokens);
    }

    void Lexer::addToken(TokenType type, Value value, std::vector<Token>& tokens) {
        std::string text = source.substr(start, current - start);
        tokens.push_back({type, text, value, line});
    }

    void Lexer::scanToken(std::vector<Token>& tokens) {
        char c = advance();
        switch (c) {
            case '(': addToken(TokenType::LEFT_PAREN, tokens); break;
            case ')': addToken(TokenType::RIGHT_PAREN, tokens); break;
            case '{': addToken(TokenType::LEFT_BRACE, tokens); break;
            case '}': addToken(TokenType::RIGHT_BRACE, tokens); break;
            case '+': addToken(TokenType::PLUS, tokens); break;
            case '-': addToken(TokenType::MINUS, tokens); break;
            case '*': addToken(TokenType::STAR, tokens); break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType::SLASH, tokens);
                }
                break;
            case '&':
                if (match('&')) addToken(TokenType::AND, tokens);
                else std::cerr << "Unexpected character: & at line " << line << std::endl;
                break;
            case '|':
                if (match('|')) addToken(TokenType::OR, tokens);
                else std::cerr << "Unexpected character: | at line " << line << std::endl;
                break;
            case ';': addToken(TokenType::SEMICOLON, tokens); break;
            case '<': addToken(TokenType::LESS, tokens); break;
            case '>': addToken(TokenType::GREATER, tokens); break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL, tokens);
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;
            default:
                if (isdigit(c)) {
                    number(tokens);
                } else if (isalpha(c)) {
                    identifier(tokens);
                } else {
                    std::cerr << "Unexpected character: " << c << " at line " << line << std::endl;
                }
                break;
        }
    }

    void Lexer::number(std::vector<Token>& tokens) {
        while (isdigit(peek())) advance();
        int value = std::stoi(source.substr(start, current - start));
        addToken(TokenType::NUMBER, value, tokens);
    }

    void Lexer::identifier(std::vector<Token>& tokens) {
        while (isalnum(peek())) advance();
        std::string text = source.substr(start, current - start);
        
        static std::map<std::string, TokenType> keywords = {
            {"let", TokenType::LET},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"print", TokenType::PRINT},
            {"input", TokenType::INPUT},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE}
        };

        if (keywords.count(text)) {
            TokenType type = keywords[text];
            if (type == TokenType::TRUE) addToken(type, true, tokens);
            else if (type == TokenType::FALSE) addToken(type, false, tokens);
            else addToken(type, tokens);
        } else {
            addToken(TokenType::IDENTIFIER, tokens);
        }
    }
}
