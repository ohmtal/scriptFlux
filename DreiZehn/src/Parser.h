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
        || peek().type == TokenType::Equal;
    }
    bool isContinuePeak() {
        return peek().type != TokenType::EOFToken
        && peek().type != TokenType::RParen
        && peek().type != TokenType::Semicolon
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

            if (DreiZehn::FunctionMap::IsFunction(nameToken.value)) {
                std::vector<std::unique_ptr<Expression>> args;
                while (isContinuePeak()) {
                    args.push_back(parseMath());

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
            || peek().type == TokenType::Equal) {
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
                    args.push_back(parseMath());
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
            if (Tools::gDumpStateNodes) {
                Tools::printf("---------- new statement ------ Pos:%d \n", (int)pos);
                for(size_t i = pos; i < tokens.size() ; i++) {
                    Tools::printf("Token %d: %d :: %s\n", i, (int)tokens[i].type, tokenTypeToString(tokens[i].type));
                }
            }
            size_t startIndex = pos;
            auto stmt = parseLine();
            if (Tools::gDumpStateNodes) {
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
            auto thenBranch = parseLine();

            return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch));
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
        }

        return parseComparison();
    }

};
} //namespace
