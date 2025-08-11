#include "parser.hpp"
#include <stdexcept>

static VarType stringToVarType(const std::string& s) {
    if (s == "Int") return VarType::Int;
    if (s == "Float") return VarType::Float;
    if (s == "String") return VarType::String;
    if (s == "Char") return VarType::Char;
    if (s == "Bool") return VarType::Bool;
    if (s == "Void") return VarType::Void;
    throw std::runtime_error("Unknown type: " + s);
}

bool Parser::isTypeToken(TokenType t) const {
    switch (t) {
        case TokenType::IntType:
        case TokenType::FloatType:
        case TokenType::CharType:
        case TokenType::BoolType:
        case TokenType::StringType:
        case TokenType::VoidType:
            return true;
        default:
            return false;
    }
}

void Parser::advance() {
    current = lexer.nextToken();
}

bool Parser::check(TokenType type) const {
    return current.type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string &msg) {
    if (!match(type)) throw std::runtime_error("Expected " + msg);
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto prog = std::make_unique<Program>();
    while (!check(TokenType::Eof)) {
        prog->functions.push_back(std::unique_ptr<Function>(static_cast<Function*>(parseFunction().release())));
    }
    return prog;
}

ASTPtr Parser::parseFunction() {
    expect(TokenType::Fn, "`fn`");
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected function name");
    std::string name = current.lexeme;
    advance();

    expect(TokenType::LParen, "`(`");
    std::vector<std::pair<std::string, VarType>> params;
    if (!check(TokenType::RParen)) {
        do {
            if (!check(TokenType::Identifier)) throw std::runtime_error("Expected parameter name");
            std::string pname = current.lexeme;
            advance();
            expect(TokenType::Colon, "`:`");
            if (!isTypeToken(current.type)) throw std::runtime_error("Expected parameter type");
            VarType ptype = stringToVarType(current.lexeme);
            advance();
            params.push_back({pname, ptype});
        } while (match(TokenType::Comma));
    }
    expect(TokenType::RParen, "`)`");
    expect(TokenType::Arrow, "`->`");
    if (!isTypeToken(current.type)) throw std::runtime_error("Expected return type");
    VarType returnType = stringToVarType(current.lexeme);
    advance();

    auto stmts = parseBlock();
    auto body = std::make_unique<BlockStmt>(std::move(stmts));

    auto fn = std::make_unique<Function>(name, returnType, std::move(params), std::move(body));
    return fn;
}

std::vector<ASTPtr> Parser::parseBlock() {
    expect(TokenType::LBrace, "`{`");
    std::vector<ASTPtr> stmts;
    while (!check(TokenType::RBrace) && !check(TokenType::Eof)) {
        stmts.push_back(parseStatement());
    }
    expect(TokenType::RBrace, "`}`");
    return stmts;
}

ASTPtr Parser::parseStatement() {
    ASTPtr stmt;

    if (match(TokenType::Let)) stmt = parseLetDecl();
    else if (match(TokenType::If)) stmt = parseIfStmt();
    else if (match(TokenType::Return)) stmt = parseReturnStmt();
    else stmt = parseExpression();

    if (!check(TokenType::RBrace)) {
        expect(TokenType::Semi, "`;` after statement");
    }
    return stmt;
}

ASTPtr Parser::parseLetDecl() {
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected variable name");
    std::string name = current.lexeme;
    advance();
    expect(TokenType::Colon, "`:`");
    if (!isTypeToken(current.type)) throw std::runtime_error("Expected type name");
    VarType type = stringToVarType(current.lexeme);
    advance();
    ASTPtr init = nullptr;
    if (match(TokenType::Eq)) {
        init = parseExpression();
    }
    return std::make_unique<LetDecl>(name, type, std::move(init));
}

ASTPtr Parser::parseIfStmt() {
    auto cond = parseExpression();
    auto thenBranch = parseBlock();
    std::vector<ASTPtr> elseBranch;
    if (match(TokenType::Else)) {
        elseBranch = parseBlock();
    }
    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBranch), std::move(elseBranch));
}

ASTPtr Parser::parseReturnStmt() {
    auto value = parseExpression();
    return std::make_unique<ReturnStmt>(std::move(value));
}

ASTPtr Parser::parseExpression() {
    return parseEquality();
}

ASTPtr Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::Eq)) {
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>("=", std::move(expr), std::move(right));
    }
    return expr;
}

ASTPtr Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::Leq)) {
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpr>("<=", std::move(expr), std::move(right));
    }
    return expr;
}

ASTPtr Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        std::string op = current.lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ASTPtr Parser::parseFactor() {
    auto expr = parsePrimary();
    while (match(TokenType::Star) || match(TokenType::Slash)) {
        std::string op = current.lexeme;
        auto right = parsePrimary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

ASTPtr Parser::parsePrimary() {
    if (check(TokenType::Integer)) {
        std::string numText = current.lexeme;
        advance();
        return std::make_unique<IntExpr>(std::stoll(numText));
    }
    if (check(TokenType::Float)) {
        std::string numText = current.lexeme;
        advance();
        return std::make_unique<DoubleExpr>(std::stof(numText));
    }
    if (check(TokenType::String)) {
        std::string strText = current.lexeme;
        advance();
        return std::make_unique<StringExpr>(strText);
    }
    if (check(TokenType::Char)) {
        std::string charText = current.lexeme;
        advance();
        if (charText.empty()) 
            throw std::runtime_error("Empty char literal");
        return std::make_unique<CharExpr>(charText[0]);
    }
    if (check(TokenType::Bool)) {
        bool val = (current.lexeme == "true");
        advance();
        return std::make_unique<BoolExpr>(val);
    }
    if (check(TokenType::Identifier)) {
        return parseCallOrVar();
    }
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "`)`");
        return expr;
    }
    if (check(TokenType::VoidType)) {
        advance();
        return std::make_unique<VoidExpr>();
    }

    throw std::runtime_error("Unexpected token in expression");
}

ASTPtr Parser::parseCallOrVar() {
    std::string name = current.lexeme;
    advance();
    if (match(TokenType::LParen)) {
        std::vector<ASTPtr> args;
        if (!check(TokenType::RParen)) {
            do {
                args.push_back(parseExpression());
            } while (match(TokenType::Comma));
        }
        expect(TokenType::RParen, "`)`");
        return std::make_unique<CallExpr>(name, std::move(args));
    }
    return std::make_unique<VarExpr>(name);
}
