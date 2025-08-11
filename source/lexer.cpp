#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string &src) : source(src), length(src.size()) {}

char Lexer::peek() const {
    if (pos >= length) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (pos >= length) return '\0';
    return source[pos++];
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        pos++;
        return true;
    }
    return false;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) advance();
}

Token Lexer::identifierOrKeyword() {
    size_t start = pos;
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, pos - start);

    if (text == "fn")    return {TokenType::Fn, text};
    if (text == "let")   return {TokenType::Let, text};
    if (text == "if")    return {TokenType::If, text};
    if (text == "else")  return {TokenType::Else, text};
    if (text == "return")return {TokenType::Return, text};

    return {TokenType::Identifier, text};
}

Token Lexer::number() {
    size_t start = pos;
    while (std::isdigit(peek())) advance();
    return {TokenType::Interger, source.substr(start, pos - start)};
}

Token Lexer::string() {
    advance();
    size_t start = pos;
    while (peek() != '"' && peek() != '\0') advance();
    if (peek() == '\0') throw std::runtime_error("Unterminated string");
    std::string value = source.substr(start, pos - start);
    advance();
    return {TokenType::String, value};
}

Token Lexer::nextToken() {
    skipWhitespace();
    if (pos >= length) return {TokenType::Eof, ""};

    char c = advance();
    switch (c) {
        case '(': return {TokenType::LParen, "("};
        case ')': return {TokenType::RParen, ")"};
        case '{': return {TokenType::LBrace, "{"};
        case '}': return {TokenType::RBrace, "}"};
        case ',': return {TokenType::Comma, ","};
        case ':': return {TokenType::Colon, ":"};
        case '+': return {TokenType::Plus, "+"};
        case '-':
            if (match('>')) return {TokenType::Arrow, "->"};
            return {TokenType::Minus, "-"};
        case '*': return {TokenType::Star, "*"};
        case '/': return {TokenType::Slash, "/"};
        case '=': return {TokenType::Eq, "="};
        case '<':
            if (match('=')) return {TokenType::Leq, "<="};
            break;
        case '"': pos--; return string();
        case ';': return { TokenType::Semi, ";"};
    }

    if (std::isalpha(c) || c == '_') {
        pos--; return identifierOrKeyword();
    }
    if (std::isdigit(c)) {
        pos--; return number();
    }

    throw std::runtime_error(std::string("Unexpected character: ") + c);
}

Token Lexer::peekToken() {
    size_t savedPos = pos;
    Token tok = nextToken();
    pos = savedPos;
    return tok;
}
