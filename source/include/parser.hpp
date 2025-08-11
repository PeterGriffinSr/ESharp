#pragma once
#include "lexer.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(Lexer &lex) : lexer(lex) {
        advance();
    }

    std::unique_ptr<Program> parseProgram();

private:
    Lexer &lexer;
    Token current;

    void advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    void expect(TokenType type, const std::string &msg);
    bool isTypeToken(TokenType t) const;

    ASTPtr parseFunction();
    ASTPtr parseStatement();
    ASTPtr parseLetDecl();
    ASTPtr parseIfStmt();
    ASTPtr parseReturnStmt();
    ASTPtr parseExpression();
    ASTPtr parseEquality();
    ASTPtr parseComparison();
    ASTPtr parseTerm();
    ASTPtr parseFactor();
    ASTPtr parsePrimary();
    ASTPtr parseCallOrVar();
    std::vector<ASTPtr> parseBlock();
};
