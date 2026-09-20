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

namespace DreiZehn{

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct AssignNode : public ASTNode {
    std::string varName;
    std::string valueStr;
    AssignNode(std::string name, std::string val) : varName(name), valueStr(val) {}
};

struct CommandNode : public ASTNode {
    std::string cmdName;
    std::vector<std::string> args;
    CommandNode(std::string name, std::vector<std::string> a) : cmdName(name), args(a) {}
};

} //namespace
