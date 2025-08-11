#pragma once
#include <string>
#include <vector>
#include <memory>

enum class VarType {
    Int,
    Float,
    String,
    Char,
    Bool,
    Void,
};

inline std::string toString(VarType t) {
    switch (t) {
        case VarType::Int: return "Int";
        case VarType::Float: return "Float";
        case VarType::String: return "String";
        case VarType::Char: return "Char";
        case VarType::Bool: return "Bool";
        case VarType::Void: return "Void";
        default: return "Unknown";
    }
}

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;
};

using ASTPtr = std::unique_ptr<ASTNode>;

struct Expr : ASTNode {};

struct IntExpr : Expr {
    int64_t value;
    explicit IntExpr(int64_t v);
    void dump(int indent = 0) const override;
};

struct DoubleExpr : Expr {
    double value;
    explicit DoubleExpr(double v);
    void dump(int indent = 0) const override;
};

struct StringExpr : Expr {
    std::string value;
    explicit StringExpr(const std::string& v);
    void dump(int indent = 0) const override;
};

struct CharExpr : Expr {
    char value;
    explicit CharExpr(char v);
    void dump(int indent = 0) const override;
};

struct BoolExpr : Expr {
    bool value;
    explicit BoolExpr(bool v);
    void dump(int indent = 0) const override;
};

struct VoidExpr : Expr {
    VoidExpr();
    void dump(int indent = 0) const override;
};

struct VarExpr : Expr {
    std::string name;
    explicit VarExpr(const std::string& n);
    void dump(int indent = 0) const override;
};

struct BinaryExpr : Expr {
    std::string op;
    ASTPtr left = nullptr;
    ASTPtr right = nullptr;
    BinaryExpr(const std::string& o, ASTPtr l, ASTPtr r);
    void dump(int indent = 0) const override;
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<ASTPtr> args;
    CallExpr(const std::string& c, std::vector<ASTPtr> a);
    void dump(int indent = 0) const override;
};

struct Stmt : ASTNode {};

struct ReturnStmt : Stmt {
    ASTPtr value;
    explicit ReturnStmt(ASTPtr v);
    void dump(int indent = 0) const override;
};

struct IfStmt : Stmt {
    ASTPtr cond = nullptr;
    std::vector<ASTPtr> thenBranch;
    std::vector<ASTPtr> elseBranch;
    IfStmt(ASTPtr condition,
           std::vector<ASTPtr> thenB,
           std::vector<ASTPtr> elseB = {});
    void dump(int indent = 0) const override;
};

struct LetDecl : Stmt {
    std::string name;
    VarType type;
    ASTPtr init = nullptr;
    LetDecl(const std::string& n, VarType t, ASTPtr i);
    void dump(int indent = 0) const override;
};

struct BlockStmt : Stmt {
    std::vector<ASTPtr> statements;
    explicit BlockStmt(std::vector<ASTPtr> stmts);
    void dump(int indent = 0) const override;
};

struct Function : Stmt {
    std::string name;
    VarType returnType;
    std::vector<std::pair<std::string, VarType>> params;
    std::unique_ptr<BlockStmt> body;
    Function(const std::string& n,
             VarType rt,
             std::vector<std::pair<std::string, VarType>> p,
             std::unique_ptr<BlockStmt> b);
    void dump(int indent = 0) const override;
};

struct Program : ASTNode {
    std::vector<std::unique_ptr<Function>> functions;
    void dump(int indent = 0) const override;
};