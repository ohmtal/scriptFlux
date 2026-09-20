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

#include "Lexer.h"
#include "AstNode.h"

namespace DreiZehn{

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() { return tokens[pos]; }
    Token advance() { return tokens[pos++]; }

public:
    Parser(std::vector<Token> t) : tokens(std::move(t)) {}

    std::unique_ptr<ASTNode> parseLine() {
        if (peek().type == TokenType::EOFToken) return nullptr;


        if (peek().type == TokenType::Identifier) {
            Token nameToken = advance();

            if (peek().type == TokenType::Assign) {
                advance(); // '='
                Token valToken = advance();

                // String:
                std::string rawVal = valToken.value;
                if (valToken.type == TokenType::StringLiteral) {
                    // add marker again
                    rawVal = "\"" + rawVal + "\"";
                }
                return std::make_unique<AssignNode>(nameToken.value, rawVal);
            }

            std::vector<std::string> args;
            while (peek().type == TokenType::Identifier ||
                peek().type == TokenType::Number ||
                peek().type == TokenType::StringLiteral) {

                Token argToken = advance();
            if (argToken.type == TokenType::StringLiteral) {
                args.push_back("\"" + argToken.value + "\"");
            } else {
                args.push_back(argToken.value);
            }
                }
                return std::make_unique<CommandNode>(nameToken.value, args);
        }


        // // if (peek().type == TokenType::Identifier) {
        // //     Token nameToken = advance();
        // //
        // //
        // //     if (peek().type == TokenType::Assign) {
        // //         advance(); // skip '='
        // //         Token valToken = advance();
        // //         return std::make_unique<AssignNode>(nameToken.value, valToken.value);
        // //     }
        // //
        // //     std::vector<std::string> args;
        // //     while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
        // //         args.push_back(advance().value);
        // //     }
        // //     return std::make_unique<CommandNode>(nameToken.value, args);
        // // }
        return nullptr;
    }
};
} //namespace
