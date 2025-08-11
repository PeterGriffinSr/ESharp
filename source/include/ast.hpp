#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void dump(int index = 0) const = 0;
};

using ASTPtr = std::shared_ptr<ASTNode>;

struct Expr : ASTNode {};

struct IntExpr : Expr {
    int value;
    IntExpr(int v) : value(v) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Int(" << value << ")\n";
    }
};

struct StringExpr : Expr {
    std::string value;
    StringExpr(std::string v) : value(std::move(v)) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "String(\"" << value << "\")\n";
    }
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Var(" << name << ")\n";
    }
};

struct BinaryExpr : Expr {
    std::string op;
    ASTPtr left, right;
    BinaryExpr(std::string o, ASTPtr l, ASTPtr r) : op(std::move(o)), left(l), right(r) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Binary(" << op << ")\n";
        left->dump(indent + 2);
        right->dump(indent + 2);
    }
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<ASTPtr> args;
    CallExpr(std::string c, std::vector<ASTPtr> a) : callee(std::move(c)), args(std::move(a)) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Call(" << callee << ")\n";
        for (auto &arg : args) arg->dump(indent + 2);
    }
};

struct Stmt : ASTNode {};

struct ReturnStmt : Stmt {
    ASTPtr value;
    ReturnStmt(ASTPtr v) : value(v) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Return\n";
        value->dump(indent + 2);
    }
};

struct IfStmt : Stmt {
    ASTPtr cond;
    std::vector<ASTPtr> thenBranch;
    std::vector<ASTPtr> elseBranch;
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "If\n";
        cond->dump(indent + 2);
        std::cout << std::string(indent, ' ') << "Then:\n";
        for (auto &s : thenBranch) s->dump(indent + 2);
        if (!elseBranch.empty()) {
            std::cout << std::string(indent, ' ') << "Else:\n";
            for (auto &s : elseBranch) s->dump(indent + 2);
        }
    }
};

struct VarDecl : Stmt {
    std::string name, type;
    ASTPtr init;
    VarDecl(std::string n, std::string t, ASTPtr i) : name(std::move(n)), type(std::move(t)), init(i) {}
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDecl(" << name << ": " << type << ")\n";
        if (init) init->dump(indent + 2);
    }
};

struct Function : Stmt {
    std::string name, returnType;
    std::vector<std::pair<std::string, std::string>> params;
    std::vector<ASTPtr> body;
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Function " << name << " -> " << returnType << "\n";
        for (auto &p : params)
            std::cout << std::string(indent + 2, ' ') << "Param: " << p.first << ": " << p.second << "\n";
        for (auto &s : body) s->dump(indent + 2);
    }
};

struct Program : ASTNode {
    std::vector<ASTPtr> functions;
    void dump(int indent = 0) const override {
        std::cout << "Program\n";
        for (auto &f : functions) f->dump(indent + 2);
    }
};