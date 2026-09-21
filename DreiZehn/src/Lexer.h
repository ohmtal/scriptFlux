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
#include "Tools.h"

namespace DreiZehn{

enum class TokenType {
    Identifier
    , Number
    , Assign
    , StringLiteral
    , LParen
    , RParen
    // math
    , Plus , Minus , Mul , Div
    // if ...
    , If, Greater, Less, Equal, Else
    // fn functions
    , Fn, End

    // for iter
     ,For

    // break, return
    , Break, Return

    , EOFToken
};

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
            // // ----------------------------------------------------------------
            if (std::isspace(peek())) { advance(); continue; }
            // ----------------------------------------------------------------

            if (peek() == '=') {
                advance();
                if (peek() == '=') { // "=="
                    advance();
                    tokens.push_back({TokenType::Equal, "=="});
                } else {
                    tokens.push_back({TokenType::Assign, "="});
                }
                continue;
            }
            if (peek() == '(') { advance(); tokens.push_back({TokenType::LParen, "("}); continue; }
            if (peek() == ')') { advance(); tokens.push_back({TokenType::RParen, ")"}); continue; }
            if (peek() == '+') { advance(); tokens.push_back({TokenType::Plus, "+"}); continue; }
            if (peek() == '-') { advance(); tokens.push_back({TokenType::Minus, "-"}); continue; }
            if (peek() == '*') { advance(); tokens.push_back({TokenType::Mul,   "*"}); continue; }
            if (peek() == '/') { advance(); tokens.push_back({TokenType::Div,   "/"}); continue; }

            // ----------------------------------------------------------------
            if (peek() == '>') { advance(); tokens.push_back({TokenType::Greater, ">"}); continue; }
            if (peek() == '<') { advance(); tokens.push_back({TokenType::Less, "<"}); continue; }
            // ----------------------------------------------------------------

            // ----------------------------------------------------------------
            // Numbers
            if (std::isdigit(peek()) || peek() == '.') {
                std::string num;
                while (std::isdigit(peek()) || peek() == '.') {
                    num += advance();
                }
                tokens.push_back({TokenType::Number, num});
                continue;
            }

            // ----------------------------------------------------------------
            // Identifier and Keywords scannen
            if (std::isalpha(peek()) || peek() == '_') {
                std::string id;

                while (std::isalnum(peek()) || peek() == '_') {
                    id += advance();
                }

                if (id == "if") { tokens.push_back({TokenType::If, "if"}); }
                else if (id == "else") { tokens.push_back({TokenType::Else, "else"}); }
                else if (id == "fn") { tokens.push_back({TokenType::Fn, "fn"});  }
                else if (id == "end") { tokens.push_back({TokenType::End, "end"}); }
                else if (id == "for") { tokens.push_back({TokenType::For, "for"});  }
                else if (id == "break") { tokens.push_back({TokenType::Break, "break"});  }
                else if (id == "return") { tokens.push_back({TokenType::Return, "return"});  }
                else {
                    tokens.push_back({TokenType::Identifier, id});
                }
                continue;
            }


            // ----------------------------------------------------------------
            // StringLiteral

            if (peek() == '"') {
                advance(); // eat '"'
                std::string strValue;

                while (peek() != '"' && peek() != '\0') {
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
                    advance(); //eat '"'
                } else {
                    Tools::errorf("[Lexer-ERROR] String not closed!\n");
                    pos = src.size();
                    continue;
                }

                tokens.push_back({TokenType::StringLiteral, strValue});
                continue;
            }

            // // if (peek() == '"') {
            // //     advance(); //skip inital '"'
            // //     std::string strValue;
            // //
            // //     while (peek() != '"' && peek() != '\0') {
            // //         // skip \" oder \n a
            // //         if (peek() == '\\') {
            // //             advance();
            // //             if (peek() == 'n') { strValue += '\n'; advance(); }
            // //             else if (peek() == '"') { strValue += '"'; advance(); }
            // //             else { strValue += '\\'; }
            // //         } else {
            // //             strValue += advance();
            // //         }
            // //     }
            // //
            // //     if (peek() == '"') {
            // //         advance(); // skip trailing '"'
            // //     } else {
            // //         Tools::errorf("[Lexer-ERROR] String not closed!\n");
            // //     }
            // //     tokens.push_back({TokenType::StringLiteral, strValue});
            // //     continue;
            // // } //STR
            // ----------------------------------------------------------------
            advance(); // skip unknown
        }
        tokens.push_back({TokenType::EOFToken, ""});
        return tokens;
    }
};
} //Namespace
