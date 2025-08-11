#include "parser.hpp"
#include <stdexcept>

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

std::shared_ptr<Program> Parser::parseProgram() {
    auto prog = std::make_shared<Program>();
    while (!check(TokenType::Eof)) {
        prog->functions.push_back(parseFunction());
    }
    return prog;
}

ASTPtr Parser::parseFunction() {
    expect(TokenType::Fn, "`fn`");
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected function name");
    std::string name = current.lexeme;
    advance();

    expect(TokenType::LParen, "`(`");
    std::vector<std::pair<std::string, std::string>> params;
    if (!check(TokenType::RParen)) {
        do {
            if (!check(TokenType::Identifier)) throw std::runtime_error("Expected parameter name");
            std::string pname = current.lexeme;
            advance();
            expect(TokenType::Colon, "`:`");
            if (!check(TokenType::Identifier)) throw std::runtime_error("Expected parameter type");
            std::string ptype = current.lexeme;
            advance();
            params.push_back({pname, ptype});
        } while (match(TokenType::Comma));
    }
    expect(TokenType::RParen, "`)`");
    expect(TokenType::Arrow, "`->`");
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected return type");
    std::string returnType = current.lexeme;
    advance();

    auto body = parseBlock();

    auto fn = std::make_shared<Function>();
    fn->name = name;
    fn->returnType = returnType;
    fn->params = params;
    fn->body = std::move(body);
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

    if (match(TokenType::Let)) stmt = parseVarDecl();
    else if (match(TokenType::If)) stmt = parseIfStmt();
    else if (match(TokenType::Return)) stmt = parseReturnStmt();
    else stmt = parseExpression();

    if (!check(TokenType::RBrace)) {
        expect(TokenType::Semi, "`;` after statement");
    }
    return stmt;
}

ASTPtr Parser::parseVarDecl() {
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected variable name");
    std::string name = current.lexeme;
    advance();
    expect(TokenType::Colon, "`:`");
    if (!check(TokenType::Identifier)) throw std::runtime_error("Expected type name");
    std::string type = current.lexeme;
    advance();
    ASTPtr init = nullptr;
    if (match(TokenType::Eq)) {
        init = parseExpression();
    }
    return std::make_shared<VarDecl>(name, type, init);
}

ASTPtr Parser::parseIfStmt() {
    auto cond = parseExpression();
    auto thenBranch = parseBlock();
    std::vector<ASTPtr> elseBranch;
    if (match(TokenType::Else)) {
        elseBranch = parseBlock();
    }
    auto ifs = std::make_shared<IfStmt>();
    ifs->cond = cond;
    ifs->thenBranch = std::move(thenBranch);
    ifs->elseBranch = std::move(elseBranch);
    return ifs;
}

ASTPtr Parser::parseReturnStmt() {
    auto value = parseExpression();
    return std::make_shared<ReturnStmt>(value);
}

ASTPtr Parser::parseExpression() {
    return parseEquality();
}

ASTPtr Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::Eq)) {
        auto right = parseComparison();
        expr = std::make_shared<BinaryExpr>("=", expr, right);
    }
    return expr;
}

ASTPtr Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::Leq)) {
        auto right = parseTerm();
        expr = std::make_shared<BinaryExpr>("<=", expr, right);
    }
    return expr;
}

ASTPtr Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        std::string op = current.lexeme;
        auto right = parseFactor();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

ASTPtr Parser::parseFactor() {
    auto expr = parsePrimary();
    while (match(TokenType::Star) || match(TokenType::Slash)) {
        std::string op = current.lexeme;
        auto right = parsePrimary();
        expr = std::make_shared<BinaryExpr>(op, expr, right);
    }
    return expr;
}

ASTPtr Parser::parsePrimary() {
    if (check(TokenType::Integer)) {
        std::string numText = current.lexeme;
        advance();
        return std::make_shared<IntExpr>(std::stoi(numText));
    }
    if (check(TokenType::String)) {
        std::string strText = current.lexeme;
        advance();
        return std::make_shared<StringExpr>(strText);
    }
    if (check(TokenType::Identifier)) {
        return parseCallOrVar();
    }
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "`)`");
        return expr;
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
        return std::make_shared<CallExpr>(name, args);
    }
    return std::make_shared<VarExpr>(name);
}
