//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

#include "Lexer.h"
#include "AstNode.h"

namespace DreiZehn{

class Parser {
private:
    std::vector<Token> mTokens;
    size_t mPos = 0;

    Token peek() { return mTokens[mPos]; }
    Token peekNext() { if (mPos + 1 < mTokens.size()) return mTokens[mPos+1]; else return Token(TokenType::EOFToken); }
    Token peekNextNext() { if (mPos + 2 < mTokens.size()) return mTokens[mPos+2]; else return Token(TokenType::EOFToken); }
    Token peekPrev() { if (mPos > 1) return mTokens[mPos-1]; else return Token(TokenType::NoToken); }
    Token advance() { if (mPos + 1 < mTokens.size()) return mTokens[mPos++]; else return Token(TokenType::EOFToken);}

    // -------------------------------------------------------------------------


    bool isMathType() {
        return
        peek().mType == TokenType::Plus
        || peek().mType == TokenType::Minus
        || peek().mType == TokenType::Mul
        || peek().mType == TokenType::Div
        || peek().mType == TokenType::Greater
        || peek().mType == TokenType::Less
        || peek().mType == TokenType::Equal
        || peek().mType == TokenType::NotEqual
        || peek().mType == TokenType::Or
        || peek().mType == TokenType::And
        || peek().mType == TokenType::LowerEqual
        || peek().mType == TokenType::GreaterEqual
        || peek().mType == TokenType::BitAnd
        || peek().mType == TokenType::BitOr
        || peek().mType == TokenType::SHL
        || peek().mType == TokenType::SHR
        ;
    }
    bool isContinuePeak() {
        return peek().mType != TokenType::EOFToken
        && peek().mType != TokenType::RParen
        && peek().mType != TokenType::Semicolon
        && peek().mType != TokenType::End
        && peek().mType != TokenType::Else
        && !isMathType();
    }
    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parsePrimary() {
        if (peek().mType == TokenType::LParen) {
            advance();

            auto expr = parseMath();

            if (peek().mType == TokenType::RParen) {
                advance(); //  ')'
            } else {
                Tools::errorf("Error: Missing closing bracket %s %d\n", __FILE__, __LINE__);
            }
            return expr;
        }

        if (peek().mType == TokenType::Number || peek().mType == TokenType::StringLiteral) {
            Token t = advance();
            return std::make_unique<LiteralExpression>(t.mType, t.mValue);
        }




        if (peek().mType == TokenType::Identifier) {
            Token nameToken = advance();

            Value* constansPointer = FunctionMap::getConstants(nameToken.mValue);
            if (constansPointer != nullptr) {
                return std::make_unique<ValueExpression>((*constansPointer));
            }

            // current peek must be the arrow
            if (peek().mType  == TokenType::Arrow
                && peekNext().mType == TokenType::Identifier
            ) {

                advance(); // eat ->
                Token methodToken = advance();

                std::vector<std::unique_ptr<Expression>> args;
                while (isContinuePeak()) {
                    size_t lastPos = mPos;
                    args.push_back(parseMath());
                    if (lastPos == mPos) {
                        Tools::PrintParseError("In method call:");
                        break;
                    }
                }
                return std::make_unique<MethodExpression>(nameToken.mValue, methodToken.mValue, std::move(args));
            }

            if (FunctionMap::IsFunction(nameToken.mValue)) {
                std::vector<std::unique_ptr<Expression>> args;
                while (isContinuePeak()) {
                    size_t lastPos = mPos;
                    args.push_back(parseMath());
                    if (lastPos == mPos) {
                       Tools::PrintParseError("In function call:");
                       break;
                    }

                }
                return std::make_unique<CallExpression>(nameToken.mValue, std::move(args));
            }

            return std::make_unique<VariableExpression>(nameToken.mValue);
        }

        return nullptr;
    }

    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseMath() {
        auto left = parsePrimary();
        while (isMathType()){
            Token op = advance();
            auto right = parsePrimary();
            left = std::make_unique<BinaryExpression>(std::move(left), op.mType, std::move(right));
        }

        return left;
    }

    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseComparison() {
        auto left = parseMath();

        while (peek().mType == TokenType::Greater
            || peek().mType == TokenType::Less
            || peek().mType == TokenType::Equal
            || peek().mType == TokenType::NotEqual
            || peek().mType == TokenType::LowerEqual
            || peek().mType == TokenType::GreaterEqual
        ) {
            Token op = advance();
            auto right = parseMath();
            left = std::make_unique<BinaryExpression>(std::move(left), op.mType, std::move(right));
        }
        return left;
    }
    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseExpression() {

        if (peek().mType == TokenType::LParen) {
            advance(); // skik '('
            auto expr = parseExpression(); // inner statement

            if (peek().mType == TokenType::RParen) {
                advance(); // skip ')'
            } else {
                Tools::errorf("Error: Missing closing bracket %s %d\n", __FILE__, __LINE__);
            }
            return expr;
        }

        if (peek().mType == TokenType::Number || peek().mType == TokenType::StringLiteral) {
            Token t = advance();
            return std::make_unique<LiteralExpression>(t.mType, t.mValue);
        }


        if (peek().mType == TokenType::Identifier) {
            Token nameToken = advance();

            if (FunctionMap::IsFunction(nameToken.mValue)) {
                std::vector<std::unique_ptr<Expression>> args;

                while (isContinuePeak())
                {
                    size_t lastPos = mPos;
                    args.push_back(parseMath());
                    if (lastPos == mPos) {
                        Tools::PrintParseError("In function call:");
                        break;
                    }
                }
                return std::make_unique<CallExpression>(nameToken.mValue, std::move(args));
            }

            return std::make_unique<VariableExpression>(nameToken.mValue);
        }

        return nullptr;
    }
    // -------------------------------------------------------------------------
public:
    Parser(std::vector<Token> t) : mTokens(std::move(t)) {}

    // -------------------------------------------------------------------------
    inline std::vector<std::unique_ptr<ASTNode>> parseStatements() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (peek().mType != TokenType::EOFToken) {
            if (peek().mType == TokenType::Semicolon) {
                advance();
                continue;
            }

            // FIXME need a toggle command
            if (Globals::gDumpStateNodes) {
                Tools::printf("---------- new statement ------ Pos:%d \n", (int)mPos);
                for(size_t i = mPos; i < mTokens.size() ; i++) {
                    Tools::printf("Token %d: %d :: %s\n", i, (int)mTokens[i].mType, tokenTypeToString(mTokens[i].mType));
                }
            }
            size_t startIndex = mPos;
            auto stmt = parseLine();
            if (Globals::gDumpStateNodes) {
                Tools::printf("---------- LINE parsed ------ Pos:%d next:%d :: %s\n", (int)mPos, (int)mTokens[mPos].mType, tokenTypeToString(mTokens[mPos].mType));
            }
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
            if (peek().mType == TokenType::Semicolon) {
                advance();
            }
            else if (mPos == startIndex) {
                Tools::errorf("Syntax-Error: Unexpected token '%s'\n", tokenTypeToString(mTokens[mPos].mType));
                advance();
            }
        }
        return statements;
    }

    // -------------------------------------------------------------------------
    inline std::unique_ptr<ASTNode> parseLine() {

        if (peek().mType == TokenType::Fn) {
            advance(); //eat fn

            if (peek().mType != TokenType::Identifier) {
                Tools::errorf("Syntax-Error: function name required after fn\n");
                return nullptr;
            }

            std::string funcName = advance().mValue;
            std::vector<std::string> params;

            while (peek().mType == TokenType::Identifier) {
                params.push_back(advance().mValue);
            }
            FunctionMap::RegisteredScriptFunctions[funcName] = { params, {} };
            return std::make_unique<FunctionDefineStartNode>(funcName);
        }
        else
        if (peek().mType == TokenType::For) {
            advance();

            if (peek().mType != TokenType::Identifier) {
                Tools::errorf("Syntax-Error: variable name after for expected\n");
                return nullptr;
            }
            std::string varName = advance().mValue;

            auto start = parseMath();
            auto end = parseMath();

            return std::make_unique<ForStatement>(varName, std::move(start), std::move(end));
        }
        else
        if (peek().mType == TokenType::While) {
            advance();
            auto condition = parseComparison();
            return std::make_unique<WhileStatement>(std::move(condition));
        }

        else
        if (peek().mType == TokenType::End) {
            advance();
            return std::make_unique<FunctionDefineEndNode>();
        }
        else
        if (peek().mType == TokenType::If) {
            advance(); // skip "if"
            auto condition = parseComparison();
            return std::make_unique<IfStatement>(std::move(condition));
        }
        else
        if (peek().mType == TokenType::Else) {
            advance(); // skip "else"
            return std::make_unique<ElseMarkerNode>();
        }
        else
        if (peek().mType == TokenType::Break) {
            advance(); // eat 'break'
            return std::make_unique<BreakStatement>();
        }
        else if (peek().mType == TokenType::Return) {
            advance(); // eat'return'

            std::unique_ptr<Expression> rhs = nullptr;

            if (peek().mType != TokenType::EOFToken &&
                peek().mType != TokenType::End &&
                peek().mType != TokenType::Semicolon &&
                peek().mType != TokenType::RParen) {
                rhs = parseComparison();
            }

            return std::make_unique<ReturnStatement>(std::move(rhs));
        }
        else
        if (peek().mType == TokenType::Identifier) {
            if (peekNext().mType == TokenType::Assign) {
                std::string varName = advance().mValue;
                advance(); // '='
                auto rhs = parseComparison();
                return std::make_unique<AssignStatement>(varName, std::move(rhs));
            }
            else if (peekNext().mType == TokenType::Arrow) {
                return parsePrimary();
            }

        }

        return parseComparison();
    }

};
} //namespace
