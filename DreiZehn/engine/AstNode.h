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

// base node -------------------------------------------------------------------
struct ASTNode {
    virtual ~ASTNode() = default;
};

// base expression -------------------------------------------------------------
struct Expression : public ASTNode {
    virtual Value evaluate(Environment& env) = 0;
};

// block statement --------------------------------------------------------------
class BlockStatement : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> mBody;
};


// constants -------------------------------------------------------------------
struct LiteralExpression : public Expression {
    TokenType mType;
    std::string mRawValue;

    LiteralExpression(TokenType t, std::string val) : mType(t), mRawValue(std::move(val)) {}

    Value evaluate(Environment& env) override;
};

// Values directly pushed in (ConstantsMap)-------------------------------------------------------------------
struct ValueExpression : public Expression {
    Value mValue;

    ValueExpression(const Value& value ) : mValue(value) {}

    inline Value evaluate(Environment& env) override {
        return mValue;
    }
};

// Variables -------------------------------------------------------------------
struct VariableExpression : public Expression {
    // std::string mName;
    uint32_t mVariableNameSymbolId = 0;
    VariableExpression(uint32_t n) : mVariableNameSymbolId(n) {}
    Value evaluate(Environment& env) override; // Liest aus env.variables
};

// function calls --------------------------------------------------------------
struct CallExpression : public Expression {
    // std::string mFuncName;
    uint32_t mFuncSymbolId = 0;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(uint32_t funcSymbolID, std::vector<std::unique_ptr<Expression>> args)
    : mFuncSymbolId(funcSymbolID), arguments(std::move(args)) {}

    Value evaluate(Environment& env) override;
};

// method/field on Pointer  calls --------------------------------------------------------------
struct MethodExpression : public Expression {
    uint32_t mPointerNameSymbolId;
    uint32_t mMethodNameSymbolId;

    std::vector<std::unique_ptr<Expression>> mArguments;

    MethodExpression(uint32_t pointerNameSymId, uint32_t methodNameSymId, std::vector<std::unique_ptr<Expression>> args)
    : mPointerNameSymbolId(pointerNameSymId), mMethodNameSymbolId(methodNameSymId), mArguments(std::move(args)) {}

    Value evaluate(Environment& env) override;
};
// Assingment ------------------------------------------------------------------
struct AssignStatement : public ASTNode {
    uint32_t mVarNameSymbolId;
    std::unique_ptr<Expression> mRhs; // Right-Hand Side

    AssignStatement(uint32_t varNameSymId, std::unique_ptr<Expression> expr)
    : mVarNameSymbolId(varNameSymId), mRhs(std::move(expr)) {}
};
// Binary ----------------------------------------------------------------------
struct BinaryExpression : public Expression {
    std::unique_ptr<Expression> mLeft;
    TokenType mOp;
    std::unique_ptr<Expression> mRight;

    BinaryExpression(std::unique_ptr<Expression> l, TokenType o, std::unique_ptr<Expression> r)
    : mLeft(std::move(l)), mOp(o), mRight(std::move(r)) {}

    Value evaluate(Environment& env) override;
};
// Binary ----------------------------------------------------------------------
struct BinaryOpExpression : public Expression {
    std::unique_ptr<Expression> mLeft;
    TokenType mOp;
    std::unique_ptr<Expression> mRight;

    BinaryOpExpression(std::unique_ptr<Expression> l, TokenType o, std::unique_ptr<Expression> r)
    : mLeft(std::move(l)), mOp(o), mRight(std::move(r)) {}

    Value evaluate(Environment& env) override;
};
// If -------------------------------------------------------------------------
// struct IfStatement : public ASTNode {
struct IfStatement : public BlockStatement {
    std::unique_ptr<Expression> mCondition;
    // body is defined in BlockStatement
    std::vector<std::shared_ptr<ASTNode>> mElseBody;
     bool mIsInElseBranch = false;

    IfStatement(std::unique_ptr<Expression> cond) : mCondition(std::move(cond)) {}

};
struct ElseMarkerNode: public ASTNode {};
// fn --------------------------------------------------------------------------
struct FunctionDefineStartNode : public ASTNode {
    // std::string mFnName;
    uint32_t mFnNameSymbolId;
    FunctionDefineStartNode(uint32_t symId) : mFnNameSymbolId(symId) {}
};

// end -------------------------------------------------------------------------
struct FunctionDefineEndNode : public ASTNode {};

// for -------------------------------------------------------------------------
struct ForStatement : public BlockStatement {
    // std::string mIteratorVarName;
    uint32_t mIteratorVarNameSymbolId = 0;
    std::unique_ptr<Expression> mStartExpr;
    std::unique_ptr<Expression> mEndExpr;

    ForStatement(uint32_t nameSymId, std::unique_ptr<Expression> start, std::unique_ptr<Expression> end)
    : mIteratorVarNameSymbolId(nameSymId), mStartExpr(std::move(start)), mEndExpr(std::move(end)) {}
};
// break -------------------------------------------------------------------------
struct BreakStatement : public ASTNode {};

// return -------------------------------------------------------------------------
struct ReturnStatement : public ASTNode {
    std::unique_ptr<Expression> mExpression;
    ReturnStatement(std::unique_ptr<Expression> expr) : mExpression(std::move(expr)) {}
};

// While -------------------------------------------------------------------------
struct WhileStatement : public BlockStatement {
    std::unique_ptr<Expression> mCondition;
    WhileStatement(std::unique_ptr<Expression> cond) : mCondition(std::move(cond)) {}
};
} //namespace
