//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// The power machine :)
//-----------------------------------------------------------------------------
#pragma once
#include "VMStructure.h"
#include <memory>

namespace DreiZehn {

class ASTCompiler {
public:

    static void compileExpression(ASTNode* node, BytecodeChunk& chunk, CompilerScope& scope) {
        if (!node) return;

        // --- literal
        if (auto* literal = dynamic_cast<LiteralExpression*>(node)) {
            Value finalValue;

            if (literal->mType == TokenType::Number) {
                char* endptr = nullptr;
                double resDouble = std::strtod(literal->mRawValue.c_str(), &endptr);

                if (literal->mRawValue.find('.') != std::string::npos) {
                    finalValue = Value(resDouble);
                } else {
                    finalValue = Value(static_cast<int>(resDouble));
                }
            }
            else if (literal->mType == TokenType::StringLiteral) {
                // TODO  VM should keep track on trick objects !
                // FIXME garbage colletion is NOT used here at the moment !!!!!!!
                auto* strObj = new StringValueObject(literal->mRawValue);
                finalValue = Value(strObj);
            }

            int constIdx = chunk.addConstant(finalValue);
            chunk.emit(OpCode::Constant, constIdx);
        }

        // --- variable
        else if (auto* varExpr = dynamic_cast<VariableExpression*>(node)) {
            int slot = scope.getOrAssignSlot(varExpr->mVariableNameSymbolId);
            chunk.emit(OpCode::GetLocal, slot);
        }

        // --- operation
        else if (auto* binary = dynamic_cast<BinaryOpExpression*>(node)) {
            compileExpression(binary->mLeft.get(), chunk, scope);
            compileExpression(binary->mRight.get(), chunk, scope);

            switch (binary->mOp) {
                case TokenType::Plus    : chunk.emit(OpCode::Add); break;
                case TokenType::Minus   : chunk.emit(OpCode::Sub); break;
                case TokenType::Mul     : chunk.emit(OpCode::Mul); break;
                case TokenType::Div     : chunk.emit(OpCode::Div); break;
                default: Tools::errorf("COMPILE ERROR BinaryOpExpression : unknown TokenType: %d", (int)binary->mOp);
            }
        }
        // --- .....
    } // compileExpression
}; //ASTCompiler
} // namespace
