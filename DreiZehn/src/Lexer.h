//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Lexer / Tokenizer
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include <cctype>

namespace DreiZehn{

enum class TokenType { Identifier, Number, Assign, StringLiteral, EOFToken };

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
private:
    std::string src;
    size_t pos = 0;

    char peek() { return pos < src.size() ? src[pos] : '\0'; }
    char advance() { return pos < src.size() ? src[pos++] : '\0'; }

public:
    Lexer(std::string source) : src(std::move(source)) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (peek() != '\0') {
            if (std::isspace(peek())) {
                advance();
                continue;
            }

            // '='
            if (peek() == '=') {
                advance();
                tokens.push_back({TokenType::Assign, "="});
                continue;
            }

            // Numbers
            if (std::isdigit(peek()) || peek() == '.') {
                std::string num;
                while (std::isdigit(peek()) || peek() == '.') {
                    num += advance();
                }
                tokens.push_back({TokenType::Number, num});
                continue;
            }

            // Identifier
            if (std::isalpha(peek()) || peek() == '_') {
                std::string id;
                while (std::isalnum(peek()) || peek() == '_') {
                    id += advance();
                }
                tokens.push_back({TokenType::Identifier, id});
                continue;
            }

            // StringLiteral
            if (peek() == '"') {
                advance(); //skip inital '"'
                std::string strValue;

                while (peek() != '"' && peek() != '\0') {
                    // skip \" oder \n a
                    if (peek() == '\\') {
                        advance();
                        if (peek() == 'n') { strValue += '\n'; advance(); }
                        else if (peek() == '"') { strValue += '"'; advance(); }
                        else { strValue += '\\'; }
                    } else {
                        strValue += advance();
                    }
                }

                if (peek() == '"') {
                    advance(); // skip trailing '"'
                } else {
                    Tools::errorf("[Lexer-ERROR] String not closed!\n");
                }

                tokens.push_back({TokenType::StringLiteral, strValue});
                continue;
            } //STR

            advance(); // skip unknown
        }
        tokens.push_back({TokenType::EOFToken, ""});
        return tokens;
    }
};
} //Namespace
