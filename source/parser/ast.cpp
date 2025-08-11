#include "ast.hpp"
#include <iostream>

IntExpr::IntExpr(int64_t v) : value(v) {}
void IntExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Int(" << value << ")\n";
}

DoubleExpr::DoubleExpr(double v) : value(v) {}
void DoubleExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Double(" << value << ")\n";
}

StringExpr::StringExpr(const std::string& v) : value(v) {}
void StringExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "String(" << value << ")\n";
}

CharExpr::CharExpr(char v) : value(v) {}
void CharExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Char('" << value << "')\n";
}

BoolExpr::BoolExpr(bool v) : value(v) {}
void BoolExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Bool(" << value << ")\n";
}

VoidExpr::VoidExpr() = default;
void VoidExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Void\n";
}

VarExpr::VarExpr(const std::string& n) : name(n) {}
void VarExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Var(" << name << ")\n";
}

BinaryExpr::BinaryExpr(const std::string& o, ASTPtr l, ASTPtr r)
    : op(o), left(std::move(l)), right(std::move(r)) {}
void BinaryExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Binary(" << op << ")\n";
    if (left) left->dump(indent + 2);
    if (right) right->dump(indent + 2);
}

CallExpr::CallExpr(const std::string& c, std::vector<ASTPtr> a)
    : callee(c), args(std::move(a)) {}
void CallExpr::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Call(" << callee << ")\n";
    for (const auto& arg : args) arg->dump(indent + 2);
}

ReturnStmt::ReturnStmt(ASTPtr v) : value(std::move(v)) {}
void ReturnStmt::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Return\n";
    if (value) value->dump(indent + 2);
}

IfStmt::IfStmt(ASTPtr condition, std::vector<ASTPtr> thenB, std::vector<ASTPtr> elseB)
    : cond(std::move(condition)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
void IfStmt::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "If\n";
    if (cond) cond->dump(indent + 2);
    std::cout << std::string(indent, ' ') << "Then:\n";
    for (const auto& s : thenBranch) s->dump(indent + 2);
    if (!elseBranch.empty()) {
        std::cout << std::string(indent, ' ') << "Else:\n";
        for (const auto& s : elseBranch) s->dump(indent + 2);
    }
}

LetDecl::LetDecl(const std::string& n, VarType t, ASTPtr i)
    : name(n), type(t), init(std::move(i)) {}
void LetDecl::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Let(" << name << ": " << toString(type) << ")\n";
    if (init) init->dump(indent + 2);
}

BlockStmt::BlockStmt(std::vector<ASTPtr> stmts)
    : statements(std::move(stmts)) {}
void BlockStmt::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Block\n";
    for (const auto& stmt : statements) stmt->dump(indent + 2);
}

Function::Function(const std::string& n, VarType rt,
                   std::vector<std::pair<std::string, VarType>> p,
                   std::unique_ptr<BlockStmt> b)
    : name(n), returnType(rt), params(std::move(p)), body(std::move(b)) {}
void Function::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Function " << name << " -> " << toString(returnType) << "\n";
    for (const auto& param : params)
        std::cout << std::string(indent + 2, ' ') << "Param: " << param.first << ": " << toString(param.second) << "\n";
    if (body) body->dump(indent + 2);
}

void Program::dump(int indent) const {
    std::cout << std::string(indent, ' ') << "Program\n";
    for (const auto& func : functions) {
        func->dump(indent + 2);
    }
}
