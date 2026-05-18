#include "parser.hpp"
#include <stdexcept>
#include <iostream>

namespace cvm {
    static void printIndent(int indent) {
        for (int i = 0; i < indent; i++) std::cout << "  ";
    }

    void LiteralExpr::print(int indent) const {
        printIndent(indent);
        if (std::holds_alternative<int>(value)) std::cout << std::get<int>(value);
        else std::cout << (std::get<bool>(value) ? "true" : "false");
        std::cout << std::endl;
    }

    void UnaryExpr::print(int indent) const {
        printIndent(indent);
        std::cout << "Unary " << op.lexeme << std::endl;
        right->print(indent + 1);
    }

    void BinaryExpr::print(int indent) const {
        printIndent(indent);
        std::cout << "Binary " << op.lexeme << std::endl;
        left->print(indent + 1);
        right->print(indent + 1);
    }

    void VariableExpr::print(int indent) const {
        printIndent(indent);
        std::cout << "Var " << name.lexeme << std::endl;
    }

    void AssignExpr::print(int indent) const {
        printIndent(indent);
        std::cout << "Assign " << name.lexeme << std::endl;
        value->print(indent + 1);
    }

    void InputExpr::print(int indent) const {
        printIndent(indent);
        std::cout << "Input" << std::endl;
    }

    void ExpressionStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "ExprStmt" << std::endl;
        expression->print(indent + 1);
    }

    void PrintStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "PrintStmt" << std::endl;
        expression->print(indent + 1);
    }

    void VarStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "VarStmt " << name.lexeme << std::endl;
        if (initializer) initializer->print(indent + 1);
    }

    void IfStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "IfStmt" << std::endl;
        condition->print(indent + 1);
        thenBranch->print(indent + 1);
        if (elseBranch) elseBranch->print(indent + 1);
    }

    void WhileStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "WhileStmt" << std::endl;
        condition->print(indent + 1);
        body->print(indent + 1);
    }

    void BlockStmt::print(int indent) const {
        printIndent(indent);
        std::cout << "BlockStmt" << std::endl;
        for (const auto& stmt : statements) {
            stmt->print(indent + 1);
        }
    }

    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    std::vector<std::unique_ptr<Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!isAtEnd()) {
            statements.push_back(statement());
        }
        return statements;
    }

    std::unique_ptr<Stmt> Parser::statement() {
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::WHILE})) return whileStatement();
        if (match({TokenType::LEFT_BRACE})) return block();
        if (match({TokenType::LET})) return varDeclaration();
        if (match({TokenType::PRINT})) return printStatement();
        return expressionStatement();
    }

    std::unique_ptr<Stmt> Parser::ifStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

        std::unique_ptr<Stmt> thenBranch = statement();
        std::unique_ptr<Stmt> elseBranch = nullptr;
        if (match({TokenType::ELSE})) {
            elseBranch = statement();
        }

        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    std::unique_ptr<Stmt> Parser::whileStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
        std::unique_ptr<Stmt> body = statement();

        return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Stmt> Parser::block() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(statement());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return std::make_unique<BlockStmt>(std::move(statements));
    }

    std::unique_ptr<Stmt> Parser::varDeclaration() {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<Expr> initializer = nullptr;
        if (match({TokenType::EQUAL})) {
            initializer = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return std::make_unique<VarStmt>(name, std::move(initializer));
    }

    std::unique_ptr<Stmt> Parser::printStatement() {
        std::unique_ptr<Expr> value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after print statement.");
        return std::make_unique<PrintStmt>(std::move(value));
    }

    std::unique_ptr<Stmt> Parser::expressionStatement() {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_unique<ExpressionStmt>(std::move(expr));
    }

    std::unique_ptr<Expr> Parser::expression() {
        std::unique_ptr<Expr> expr = or_();
        if (match({TokenType::EQUAL})) {
            Token equals = previous();
            std::unique_ptr<Expr> value = expression();

            if (auto v = dynamic_cast<VariableExpr*>(expr.get())) {
                Token name = v->name;
                return std::make_unique<AssignExpr>(name, std::move(value));
            }

            throw std::runtime_error("Invalid assignment target.");
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::or_() {
        std::unique_ptr<Expr> expr = and_();
        while (match({TokenType::OR})) {
            Token op = previous();
            std::unique_ptr<Expr> right = and_();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::and_() {
        std::unique_ptr<Expr> expr = equality();
        while (match({TokenType::AND})) {
            Token op = previous();
            std::unique_ptr<Expr> right = equality();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::equality() {
        std::unique_ptr<Expr> expr = comparison();
        while (match({TokenType::EQUAL_EQUAL})) {
            Token op = previous();
            std::unique_ptr<Expr> right = comparison();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::comparison() {
        std::unique_ptr<Expr> expr = term();
        while (match({TokenType::LESS, TokenType::GREATER})) {
            Token op = previous();
            std::unique_ptr<Expr> right = term();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::term() {
        std::unique_ptr<Expr> expr = factor();
        while (match({TokenType::PLUS, TokenType::MINUS})) {
            Token op = previous();
            std::unique_ptr<Expr> right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::factor() {
        std::unique_ptr<Expr> expr = unary();
        while (match({TokenType::STAR, TokenType::SLASH})) {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::unary() {
        if (match({TokenType::MINUS})) {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            return std::make_unique<UnaryExpr>(op, std::move(right));
        }
        return primary();
    }

    std::unique_ptr<Expr> Parser::primary() {
        if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(false);
        if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(true);
        if (match({TokenType::INPUT})) return std::make_unique<InputExpr>();
        if (match({TokenType::NUMBER})) return std::make_unique<LiteralExpr>(previous().value);

        if (match({TokenType::IDENTIFIER})) {
            return std::make_unique<VariableExpr>(previous());
        }

        if (match({TokenType::LEFT_PAREN})) {
            std::unique_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        throw std::runtime_error("Expect expression.");
    }

    bool Parser::match(std::vector<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool Parser::check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token Parser::advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool Parser::isAtEnd() const {
        return peek().type == TokenType::END_OF_FILE;
    }

    Token Parser::peek() const {
        return tokens[current];
    }

    Token Parser::previous() const {
        return tokens[current - 1];
    }

    Token Parser::consume(TokenType type, std::string message) {
        if (check(type)) return advance();
        throw std::runtime_error(message);
    }
}
