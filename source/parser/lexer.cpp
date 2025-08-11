#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string &src)
    : source(src), length(src.size()), pos(0), line(1), col(1) {}

char Lexer::peek(size_t offset) const {
    if (pos + offset >= length) return '\0';
    return source[pos + offset];
}

char Lexer::advance() {
    if (pos >= length) return '\0';
    char c = source[pos++];

    if (c == '\n') {
        line++;
        col = 1;
    } else if (c == '\t') {
        int tabSize = 4;
        col += tabSize - ((col - 1) % tabSize);
    } else {
        col++;
    }

    return c;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            while (peek() != '\n' && peek() != '\0') advance();
        } else if (c == '/' && peek(1) == '*') {
            advance(); advance();
            while (!(peek() == '*' && peek(1) == '/')) {
                if (peek() == '\0')
                    throw error("Unterminated block comment");
                advance();
            }
            advance(); advance();
        } else {
            break;
        }
    }
}

Token Lexer::identifierOrKeyword() {
    size_t startPos = pos;
    int startCol = col, startLine = line;
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(startPos, pos - startPos);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::Fn}, {"let", TokenType::Let},
        {"if", TokenType::If}, {"else", TokenType::Else},
        {"return", TokenType::Return}
    };

    auto it = keywords.find(text);
    if (it != keywords.end())
        return {it->second, text, startLine, startCol};
    return {TokenType::Identifier, text, startLine, startCol};
}

Token Lexer::number() {
    size_t startPos = pos;
    int startCol = col, startLine = line;
    bool isFloat = false;

    while (std::isdigit(peek())) advance();
    if (peek() == '.' && std::isdigit(peek(1))) {
        isFloat = true;
        advance();
        while (std::isdigit(peek())) advance();
    }

    std::string num = source.substr(startPos, pos - startPos);
    return {isFloat ? TokenType::Float : TokenType::Integer, num, startLine, startCol};
}

Token Lexer::string() {
    int startCol = col, startLine = line;
    advance();
    std::string value;
    while (true) {
        char c = advance();
        if (c == '\0') throw error("Unterminated string");
        if (c == '"') break;
        if (c == '\\') {
            char e = advance();
            switch (e) {
                case 'n': value.push_back('\n'); break;
                case 't': value.push_back('\t'); break;
                case '\\': value.push_back('\\'); break;
                case '"': value.push_back('"'); break;
                default: throw error("Invalid escape sequence");
            }
        } else {
            value.push_back(c);
        }
    }
    return {TokenType::String, value, startLine, startCol};
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    if (pos >= length) return {TokenType::Eof, "", line, col};

    char c = advance();
    int startLine = line, startCol = col - 1;

    switch (c) {
        case '(': return {TokenType::LParen, "(", startLine, startCol};
        case ')': return {TokenType::RParen, ")", startLine, startCol};
        case '{': return {TokenType::LBrace, "{", startLine, startCol};
        case '}': return {TokenType::RBrace, "}", startLine, startCol};
        case ',': return {TokenType::Comma, ",", startLine, startCol};
        case ':': return {TokenType::Colon, ":", startLine, startCol};
        case ';': return {TokenType::Semi, ";", startLine, startCol};

        case '+':
            if (match('=')) return {TokenType::PlusAssign, "+=", startLine, startCol};
            return {TokenType::Plus, "+", startLine, startCol};
        case '-':
            if (match('>')) return {TokenType::Arrow, "->", startLine, startCol};
            if (match('=')) return {TokenType::MinusAssign, "-=", startLine, startCol};
            return {TokenType::Minus, "-", startLine, startCol};
        case '*':
            if (match('=')) return {TokenType::StarAssign, "*=", startLine, startCol};
            return {TokenType::Star, "*", startLine, startCol};
        case '/':
            if (match('=')) return {TokenType::SlashAssign, "/=", startLine, startCol};
            return {TokenType::Slash, "/", startLine, startCol};

        case '=':
            if (match('=')) return {TokenType::EqEq, "==", startLine, startCol};
            return {TokenType::Eq, "=", startLine, startCol};
        case '!':
            if (match('=')) return {TokenType::Neq, "!=", startLine, startCol};
            return {TokenType::Bang, "!", startLine, startCol};
        case '<':
            if (match('=')) return {TokenType::Leq, "<=", startLine, startCol};
            return {TokenType::Less, "<", startLine, startCol};
        case '>':
            if (match('=')) return {TokenType::Geq, ">=", startLine, startCol};
            return {TokenType::Greater, ">", startLine, startCol};

        case '"': return string();
    }

    if (std::isalpha(c) || c == '_') {
        pos--; return identifierOrKeyword();
    }
    if (std::isdigit(c)) {
        pos--; return number();
    }

    throw error(std::string("Unexpected character: ") + c);
}

Token Lexer::peekToken() {
    size_t savedPos = pos;
    int savedLine = line, savedCol = col;
    Token tok = nextToken();
    pos = savedPos; line = savedLine; col = savedCol;
    return tok;
}

std::string Lexer::getCurrentLine() const {
    if (pos == 0) return "";

    size_t lineStart = pos - 1;
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
        lineStart--;
    }
    size_t lineEnd = lineStart;
    while (lineEnd < length && source[lineEnd] != '\n') {
        lineEnd++;
    }
    return source.substr(lineStart, lineEnd - lineStart);
}

LexerError Lexer::error(const std::string &msg) const {
    std::string lineText = getCurrentLine();
    int errorCol = (col > 1) ? col - 1 : 1;
    return LexerError(msg, line, errorCol, lineText);
}
