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
    , If, Greater, Less, Equal, NotEqual, Else
    // fn functions
    , Fn, End

    // for iter
     ,For

    // break, return
    , Break, Return

    , Semicolon

    , While

    , EOFToken
};

inline const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Identifier:    return "Identifier";
        case TokenType::Number:        return "Number";
        case TokenType::Assign:        return "Assign";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::LParen:        return "LParen";
        case TokenType::RParen:        return "RParen";

        // math
        case TokenType::Plus:          return "Plus";
        case TokenType::Minus:         return "Minus";
        case TokenType::Mul:           return "Mul";
        case TokenType::Div:           return "Div";

        // if ...
        case TokenType::If:            return "If";
        case TokenType::Greater:       return "Greater";
        case TokenType::Less:          return "Less";
        case TokenType::Equal:         return "Equal";
        case TokenType::NotEqual:      return "Not Equal";
        case TokenType::Else:          return "Else";

        // fn functions
        case TokenType::Fn:            return "Fn";
        case TokenType::End:           return "End";

        // for / while iter
        case TokenType::For:           return "For";
        case TokenType::While:         return "While";

        // break, return
        case TokenType::Break:         return "Break";
        case TokenType::Return:        return "Return";

        case TokenType::Semicolon:     return "Semicolon";
        case TokenType::EOFToken:      return "EOFToken";

        default:                       return "UnknownToken";
    }
}

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
private:
    std::string src;
    int32_t pos = 0;

    char peek() { return pos < src.size() ? src[pos] : '\0'; }
    char peekNext() { return pos+1 < src.size() ? src[pos + 1] : '\0'; }
    char peekPrev() { return pos-1 > 0  ? src[pos - 1] : '\0'; }
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
            if (peek() == '!' && peekNext() == '=') {
                advance();
                tokens.push_back({TokenType::NotEqual, "!="});
            }


            if (peek() == '(') { advance(); tokens.push_back({TokenType::LParen, "("}); continue; }
            if (peek() == ')') { advance(); tokens.push_back({TokenType::RParen, ")"}); continue; }
            if (peek() == '+') { advance(); tokens.push_back({TokenType::Plus, "+"}); continue; }
            if (peek() == '*') { advance(); tokens.push_back({TokenType::Mul,   "*"}); continue; }
            if (peek() == '/') { advance(); tokens.push_back({TokenType::Div,   "/"}); continue; }

            // ----------------------------------------------------------------
            if (peek() == '>') { advance(); tokens.push_back({TokenType::Greater, ">"}); continue; }
            if (peek() == '<') { advance(); tokens.push_back({TokenType::Less, "<"}); continue; }
            // ----------------------------------------------------------------

            // ----------------------------------------------------------------
            // Numbers
            //FIXME this can be optimized!
            if ( std::isdigit(peek())
                || (!std::isdigit(peekPrev()) && peek() == '-' && std::isdigit(peekNext()))
                || (std::isdigit(peekPrev()) && peek() == '.' && std::isdigit(peekNext()))
            ){
                std::string num;
                while (std::isdigit(peek()) || peek() == '.' || peek() == '-') {
                    num += advance();
                }
                tokens.push_back({TokenType::Number, num});
                continue;
            }

            // minus after number
            if (peek() == '-') { advance(); tokens.push_back({TokenType::Minus, "-"}); continue; }

            // ----------------------------------------------------------------
            // Identifier and Keywords scan
            if (std::isalpha(peek()) || peek() == '_' || peek() == '.' || peek() == ':') {
                std::string id;

                while (std::isalnum(peek()) || peek() == '_' || peek() == '.' || peek() == ':') {
                    id += advance();
                }

                if (id == "if") { tokens.push_back({TokenType::If, "if"}); }
                else if (id == "else") { tokens.push_back({TokenType::Else, "else"}); }
                else if (id == "fn") { tokens.push_back({TokenType::Fn, "fn"});  }
                else if (id == "end") { tokens.push_back({TokenType::End, "end"}); }
                else if (id == "for") { tokens.push_back({TokenType::For, "for"});  }
                else if (id == "break") { tokens.push_back({TokenType::Break, "break"});  }
                else if (id == "return") { tokens.push_back({TokenType::Return, "return"});  }
                else if (id == "while") { tokens.push_back({TokenType::While, "while"});  }
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


            if (peek() == ';') { advance(); tokens.push_back({TokenType::Semicolon, ";"}); continue; }

            advance(); // skip unknown
        }
        tokens.push_back({TokenType::EOFToken, ""});
        return tokens;
    }
};
} //Namespace
