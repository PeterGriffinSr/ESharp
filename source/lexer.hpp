#pragma once
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <stdexcept>

enum class TokenType {
    Fn, Let, Return, If, Else, Print,
    Identifier, Interger, String,
    Colon, Arrow, Eq, Leq, Plus, Minus, Star, Slash,
    LParen, RParen, LBrace, RBrace, Semi, Comma,
    Eof
};

struct Token {
    TokenType type;
    std::string lexeme;
};

class Lexer {
public:
    explicit Lexer(const std::string &source);

    Token nextToken();
    Token peekToken();

private:
    std::string source;
    size_t pos = 0;
    size_t length;
    Token current;

    char peek() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();
    Token identifierOrKeyword();
    Token number();
    Token string();
};