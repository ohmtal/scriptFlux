//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// AstNode
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Value.h"

namespace DreiZehn {

class Environment;
class Value;

// base node -------------------------------------------------------------------
struct ASTNode {
    virtual ~ASTNode() = default;
};

// base expression -------------------------------------------------------------
struct Expression : public ASTNode {
    virtual Value evaluate(Environment& env) = 0;
};


// constants -------------------------------------------------------------------
struct LiteralExpression : public Expression {
    TokenType type;
    std::string rawValue;

    LiteralExpression(TokenType t, std::string val) : type(t), rawValue(std::move(val)) {}

    Value evaluate(Environment& env) override;
};

// Variables -------------------------------------------------------------------
struct VariableExpression : public Expression {
    std::string name;
    VariableExpression(std::string n) : name(n) {}
    Value evaluate(Environment& env) override; // Liest aus env.variables
};

// function calls --------------------------------------------------------------
struct CallExpression : public Expression {
    std::string funcName;
    std::vector<std::unique_ptr<Expression>> arguments; // Argumente sind JETZT selbst Ausdrücke!

    CallExpression(std::string name, std::vector<std::unique_ptr<Expression>> args)
    : funcName(name), arguments(std::move(args)) {}

    Value evaluate(Environment& env) override;
};

// Assingment ------------------------------------------------------------------
struct AssignStatement : public ASTNode {
    std::string varName;
    std::unique_ptr<Expression> rhs; // Right-Hand Side

    AssignStatement(std::string name, std::unique_ptr<Expression> expr)
    : varName(name), rhs(std::move(expr)) {}
};
// Binary ----------------------------------------------------------------------
struct BinaryExpression : public Expression {
    std::unique_ptr<Expression> left;
    TokenType op;
    std::unique_ptr<Expression> right;

    BinaryExpression(std::unique_ptr<Expression> l, TokenType o, std::unique_ptr<Expression> r)
    : left(std::move(l)), op(o), right(std::move(r)) {}

    Value evaluate(Environment& env) override;
};
// If -------------------------------------------------------------------------
struct IfStatement : public ASTNode {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<ASTNode> thenBranch;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<ASTNode> thenB)
    : condition(std::move(cond)), thenBranch(std::move(thenB)) {}
};

// fn --------------------------------------------------------------------------
struct FunctionDefineStartNode : public ASTNode {
    std::string name;
    FunctionDefineStartNode(std::string n) : name(n) {}
};

// end -------------------------------------------------------------------------
struct FunctionDefineEndNode : public ASTNode {};

// for -------------------------------------------------------------------------
struct ForStatement : public ASTNode {
    std::string iteratorName;
    std::unique_ptr<Expression> startExpr;
    std::unique_ptr<Expression> endExpr;
    std::vector<std::shared_ptr<ASTNode>> body;

    ForStatement(std::string name, std::unique_ptr<Expression> start, std::unique_ptr<Expression> end)
    : iteratorName(name), startExpr(std::move(start)), endExpr(std::move(end)) {}
};
// break -------------------------------------------------------------------------
struct BreakStatement : public ASTNode {};

// return -------------------------------------------------------------------------
struct ReturnStatement : public ASTNode {
    std::unique_ptr<Expression> expression; // Kann nullptr sein bei "nacktem" return
    ReturnStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) {}
};
} //namespace
