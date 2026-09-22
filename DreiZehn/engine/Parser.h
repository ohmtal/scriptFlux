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
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() { return tokens[pos]; }
    Token peekNext() { if (pos + 1 < tokens.size()) return tokens[pos+1]; else return Token(TokenType::EOFToken); }
    Token peekNextNext() { if (pos + 2 < tokens.size()) return tokens[pos+2]; else return Token(TokenType::EOFToken); }
    Token peekPrev() { if (pos > 1) return tokens[pos-1]; else return Token(TokenType::NoToken); }
    Token advance() { if (pos + 1 < tokens.size()) return tokens[pos++]; else return Token(TokenType::EOFToken);}

    // -------------------------------------------------------------------------


    bool isMathType() {
        return
        peek().type == TokenType::Plus
        || peek().type == TokenType::Minus
        || peek().type == TokenType::Mul
        || peek().type == TokenType::Div
        || peek().type == TokenType::Greater
        || peek().type == TokenType::Less
        || peek().type == TokenType::Equal
        || peek().type == TokenType::NotEqual
        || peek().type == TokenType::Or
        || peek().type == TokenType::And
        || peek().type == TokenType::LowerEqual
        || peek().type == TokenType::GreaterEqual
        || peek().type == TokenType::BitAnd
        || peek().type == TokenType::BitOr
        || peek().type == TokenType::SHL
        || peek().type == TokenType::SHR
        ;
    }
    bool isContinuePeak() {
        return peek().type != TokenType::EOFToken
        && peek().type != TokenType::RParen
        && peek().type != TokenType::Semicolon
        && peek().type != TokenType::End
        && peek().type != TokenType::Else
        && !isMathType();
    }
    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parsePrimary() {
        if (peek().type == TokenType::LParen) {
            advance();

            auto expr = parseMath();

            if (peek().type == TokenType::RParen) {
                advance(); //  ')'
            } else {
                Tools::errorf("Error: Missing closing bracket %s %d\n", __FILE__, __LINE__);
            }
            return expr;
        }

        if (peek().type == TokenType::Number || peek().type == TokenType::StringLiteral) {
            Token t = advance();
            return std::make_unique<LiteralExpression>(t.type, t.value);
        }




        if (peek().type == TokenType::Identifier) {
            Token nameToken = advance();

            Value* constansPointer = FunctionMap::getConstants(nameToken.value);
            if (constansPointer != nullptr) {
                return std::make_unique<ValueExpression>((*constansPointer));
            }

            // current peek must be the arrow
            if (peek().type  == TokenType::Arrow
                && peekNext().type == TokenType::Identifier
            ) {

                advance(); // eat ->
                Token methodToken = advance();

                std::vector<std::unique_ptr<Expression>> args;
                while (isContinuePeak()) {
                    size_t lastPos = pos;
                    args.push_back(parseMath());
                    if (lastPos == pos) {
                        Tools::PrintParseError("In method call:");
                        break;
                    }
                }
                return std::make_unique<MethodExpression>(nameToken.value, methodToken.value, std::move(args));
            }

            if (FunctionMap::IsFunction(nameToken.value)) {
                std::vector<std::unique_ptr<Expression>> args;
                while (isContinuePeak()) {
                    size_t lastPos = pos;
                    args.push_back(parseMath());
                    if (lastPos == pos) {
                       Tools::PrintParseError("In function call:");
                       break;
                    }

                }
                return std::make_unique<CallExpression>(nameToken.value, std::move(args));
            }

            return std::make_unique<VariableExpression>(nameToken.value);
        }

        return nullptr;
    }

    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseMath() {
        auto left = parsePrimary();
        while (isMathType()){
            Token op = advance();
            auto right = parsePrimary();
            left = std::make_unique<BinaryExpression>(std::move(left), op.type, std::move(right));
        }

        return left;
    }

    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseComparison() {
        auto left = parseMath();

        while (peek().type == TokenType::Greater
            || peek().type == TokenType::Less
            || peek().type == TokenType::Equal
            || peek().type == TokenType::NotEqual
            || peek().type == TokenType::LowerEqual
            || peek().type == TokenType::GreaterEqual
        ) {
            Token op = advance();
            auto right = parseMath();
            left = std::make_unique<BinaryExpression>(std::move(left), op.type, std::move(right));
        }
        return left;
    }
    // -------------------------------------------------------------------------
    std::unique_ptr<Expression> parseExpression() {

        if (peek().type == TokenType::LParen) {
            advance(); // skik '('
            auto expr = parseExpression(); // inner statement

            if (peek().type == TokenType::RParen) {
                advance(); // skip ')'
            } else {
                Tools::errorf("Error: Missing closing bracket %s %d\n", __FILE__, __LINE__);
            }
            return expr;
        }

        if (peek().type == TokenType::Number || peek().type == TokenType::StringLiteral) {
            Token t = advance();
            return std::make_unique<LiteralExpression>(t.type, t.value);
        }


        if (peek().type == TokenType::Identifier) {
            Token nameToken = advance();

            if (FunctionMap::IsFunction(nameToken.value)) {
                std::vector<std::unique_ptr<Expression>> args;

                while (isContinuePeak())
                {
                    size_t lastPos = pos;
                    args.push_back(parseMath());
                    if (lastPos == pos) {
                        Tools::PrintParseError("In function call:");
                        break;
                    }
                }
                return std::make_unique<CallExpression>(nameToken.value, std::move(args));
            }

            return std::make_unique<VariableExpression>(nameToken.value);
        }

        return nullptr;
    }
    // -------------------------------------------------------------------------
public:
    Parser(std::vector<Token> t) : tokens(std::move(t)) {}

    // -------------------------------------------------------------------------
    inline std::vector<std::unique_ptr<ASTNode>> parseStatements() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (peek().type != TokenType::EOFToken) {
            if (peek().type == TokenType::Semicolon) {
                advance();
                continue;
            }

            // FIXME need a toggle command
            if (Globals::gDumpStateNodes) {
                Tools::printf("---------- new statement ------ Pos:%d \n", (int)pos);
                for(size_t i = pos; i < tokens.size() ; i++) {
                    Tools::printf("Token %d: %d :: %s\n", i, (int)tokens[i].type, tokenTypeToString(tokens[i].type));
                }
            }
            size_t startIndex = pos;
            auto stmt = parseLine();
            if (Globals::gDumpStateNodes) {
                Tools::printf("---------- LINE parsed ------ Pos:%d next:%d :: %s\n", (int)pos, (int)tokens[pos].type, tokenTypeToString(tokens[pos].type));
            }
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
            if (peek().type == TokenType::Semicolon) {
                advance();
            }
            else if (pos == startIndex) {
                Tools::errorf("Syntax-Error: Unexpected token '%s'\n", tokenTypeToString(tokens[pos].type));
                advance();
            }
        }
        return statements;
    }

    // -------------------------------------------------------------------------
    inline std::unique_ptr<ASTNode> parseLine() {

        if (peek().type == TokenType::Fn) {
            advance(); //eat fn

            if (peek().type != TokenType::Identifier) {
                Tools::errorf("Syntax-Error: function name required after fn\n");
                return nullptr;
            }

            std::string funcName = advance().value;
            std::vector<std::string> params;

            while (peek().type == TokenType::Identifier) {
                params.push_back(advance().value);
            }
            FunctionMap::RegisteredScriptFunctions[funcName] = { params, {} };
            return std::make_unique<FunctionDefineStartNode>(funcName);
        }
        else
        if (peek().type == TokenType::For) {
            advance();

            if (peek().type != TokenType::Identifier) {
                Tools::errorf("Syntax-Error: variable name after for expected\n");
                return nullptr;
            }
            std::string varName = advance().value;

            auto start = parseMath();
            auto end = parseMath();

            return std::make_unique<ForStatement>(varName, std::move(start), std::move(end));
        }
        else
        if (peek().type == TokenType::While) {
            advance();
            auto condition = parseComparison();
            return std::make_unique<WhileStatement>(std::move(condition));
        }

        else
        if (peek().type == TokenType::End) {
            advance();
            return std::make_unique<FunctionDefineEndNode>();
        }
        else
        if (peek().type == TokenType::If) {
            advance(); // skip "if"
            auto condition = parseComparison();
            return std::make_unique<IfStatement>(std::move(condition));
        }
        else
        if (peek().type == TokenType::Else) {
            advance(); // skip "else"
            return std::make_unique<ElseMarkerNode>();
        }
        else
        if (peek().type == TokenType::Break) {
            advance(); // eat 'break'
            return std::make_unique<BreakStatement>();
        }
        else if (peek().type == TokenType::Return) {
            advance(); // eat'return'

            std::unique_ptr<Expression> rhs = nullptr;

            if (peek().type != TokenType::EOFToken &&
                peek().type != TokenType::End &&
                peek().type != TokenType::Semicolon &&
                peek().type != TokenType::RParen) {
                rhs = parseComparison();
            }

            return std::make_unique<ReturnStatement>(std::move(rhs));
        }
        else
        if (peek().type == TokenType::Identifier) {
            if (peekNext().type == TokenType::Assign) {
                std::string varName = advance().value;
                advance(); // '='
                auto rhs = parseComparison();
                return std::make_unique<AssignStatement>(varName, std::move(rhs));
            }
            else if (peekNext().type == TokenType::Arrow) {
                return parsePrimary();
            }

        }

        return parseComparison();
    }

};
} //namespace
