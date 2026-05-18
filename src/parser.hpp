#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include <vector>

namespace cvm {
    // Forward declarations
    struct Expr;
    struct Stmt;

    // Expression types
    struct Expr {
        virtual ~Expr() = default;
        virtual void print(int indent = 0) const = 0;
    };

    struct LiteralExpr : Expr {
        Value value;
        LiteralExpr(Value value) : value(value) {}
        void print(int indent = 0) const override;
    };

    struct UnaryExpr : Expr {
        Token op;
        std::unique_ptr<Expr> right;
        UnaryExpr(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right)) {}
        void print(int indent = 0) const override;
    };

    struct BinaryExpr : Expr {
        std::unique_ptr<Expr> left;
        Token op;
        std::unique_ptr<Expr> right;
        BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) 
            : left(std::move(left)), op(op), right(std::move(right)) {}
        void print(int indent = 0) const override;
    };

    struct VariableExpr : Expr {
        Token name;
        VariableExpr(Token name) : name(name) {}
        void print(int indent = 0) const override;
    };

    struct AssignExpr : Expr {
        Token name;
        std::unique_ptr<Expr> value;
        AssignExpr(Token name, std::unique_ptr<Expr> value) : name(name), value(std::move(value)) {}
        void print(int indent = 0) const override;
    };

    // Statement types
    struct Stmt {
        virtual ~Stmt() = default;
        virtual void print(int indent = 0) const = 0;
    };

    struct ExpressionStmt : Stmt {
        std::unique_ptr<Expr> expression;
        ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
        void print(int indent = 0) const override;
    };

    struct PrintStmt : Stmt {
        std::unique_ptr<Expr> expression;
        PrintStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
        void print(int indent = 0) const override;
    };

    struct VarStmt : Stmt {
        Token name;
        std::unique_ptr<Expr> initializer;
        VarStmt(Token name, std::unique_ptr<Expr> initializer) : name(name), initializer(std::move(initializer)) {}
        void print(int indent = 0) const override;
    };

    struct IfStmt : Stmt {
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> thenBranch;
        std::unique_ptr<Stmt> elseBranch;
        IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
            : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
        void print(int indent = 0) const override;
    };

    struct WhileStmt : Stmt {
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Stmt> body;
        WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}
        void print(int indent = 0) const override;
    };

    struct BlockStmt : Stmt {
        std::vector<std::unique_ptr<Stmt>> statements;
        BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
        void print(int indent = 0) const override;
    };

    struct InputExpr : Expr {
        InputExpr() {}
        void print(int indent = 0) const override;
    };

    class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        std::vector<std::unique_ptr<Stmt>> parse();

    private:
        std::vector<Token> tokens;
        size_t current = 0;

        std::unique_ptr<Stmt> statement();
        std::unique_ptr<Stmt> ifStatement();
        std::unique_ptr<Stmt> whileStatement();
        std::unique_ptr<Stmt> block();
        std::unique_ptr<Stmt> varDeclaration();
        std::unique_ptr<Stmt> printStatement();
        std::unique_ptr<Stmt> expressionStatement();

        std::unique_ptr<Expr> expression();
        std::unique_ptr<Expr> or_();
        std::unique_ptr<Expr> and_();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> comparison();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> primary();

        bool match(std::vector<TokenType> types);
        bool check(TokenType type) const;
        Token advance();
        bool isAtEnd() const;
        Token peek() const;
        Token previous() const;
        Token consume(TokenType type, std::string message);
    };
}

#endif // PARSER_HPP
