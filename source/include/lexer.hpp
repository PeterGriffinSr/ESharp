#pragma once
#include "error.hpp"
#include <string>
#include <stdexcept>
#include <unordered_map>

enum class TokenType {
    Fn, Let, Return, If, Else, Print,
    Identifier, Integer, Float, String, Char, Bool,
    IntType, FloatType, StringType, CharType, BoolType, VoidType,
    Colon, Arrow, Eq, EqEq, Neq, Leq, Geq,
    Plus, Minus, Star, Slash, Bang,
    PlusAssign, MinusAssign, StarAssign, SlashAssign,
    Less, Greater,
    LParen, RParen, LBrace, RBrace, Semi, Comma,
    Eof
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int col;
};

class Lexer {
public:
    explicit Lexer(const std::string &source);

    Token nextToken();
    Token peekToken();

private:
    std::string source;
    size_t length;
    size_t pos = 0;
    int line = 1;
    int col = 1;

    char peek(size_t offset = 0) const;
    char advance();
    bool match(char expected);

    void skipWhitespaceAndComments();

    Token identifierOrKeyword();
    Token number();
    Token string();
    Token _char();

    LexerError error(const std::string &msg) const;
    std::string getCurrentLine() const;
};
