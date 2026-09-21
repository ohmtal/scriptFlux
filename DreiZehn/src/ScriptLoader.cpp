//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Loader
//-----------------------------------------------------------------------------
#include <fstream>
#include <string>
#include <vector>

#include "Environment.h"
#include "Tools.h"
#include "Parser.h"

namespace DreiZehn {

    bool RunScriptFile(const std::string& filename, Environment& env) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Tools::errorf("Script file cant be opened: %s\n", filename.c_str());
            return false;
        }

        std::string line;
        std::vector<OpenBlock> blockStack;
        std::unordered_map<ForStatement*, std::shared_ptr<ForStatement>> globalForKeeper;

        int lineCount = 0;

        while (std::getline(file, line)) {
            lineCount++;
            if (line.empty() || line[0] == '#') continue;

            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto ast = parser.parseLine();

            if (!ast) continue;

            // --- fn ---
            if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
                blockStack.push_back({BlockType::Function, startNode->name, nullptr});
                continue;
            }

            // --- for ---
            if (dynamic_cast<ForStatement*>(ast.get())) {
                std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                std::shared_ptr<ForStatement> sharedFor = std::static_pointer_cast<ForStatement>(sharedBase);

                blockStack.push_back({BlockType::ForLoop, "", sharedFor.get()});

                if (blockStack.size() > 1) {
                    auto& outerBlock = blockStack[blockStack.size() - 2];
                    if (outerBlock.type == BlockType::Function) {
                        FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedFor);
                    } else if (outerBlock.type == BlockType::ForLoop) {
                        outerBlock.forNodePointer->body.push_back(sharedFor);
                    }
                } else {
                    globalForKeeper[sharedFor.get()] = sharedFor;
                }
                continue;
            }

            // ---  end ---
            if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
                if (blockStack.empty()) {
                    Tools::errorf("[Zeile %d] Syntax-Fehler: 'end' ohne Block.\n", lineCount);
                    return false;
                }

                OpenBlock closingBlock = blockStack.back();
                blockStack.pop_back();

                if (closingBlock.type == BlockType::ForLoop && blockStack.empty()) {
                    env.execute(closingBlock.forNodePointer);
                }
                continue;
            }

            // --- command ---
            if (!blockStack.empty()) {
                auto& currentBlock = blockStack.back();
                std::shared_ptr<ASTNode> sharedAst = std::move(ast);

                if (currentBlock.type == BlockType::Function) {
                    FunctionMap::RegisteredScriptFunctions[currentBlock.funcName].body.push_back(sharedAst);
                } else if (currentBlock.type == BlockType::ForLoop) {
                    currentBlock.forNodePointer->body.push_back(sharedAst);
                }
            } else {
                env.execute(ast.get());
            }
        }

        if (!blockStack.empty()) {
            Tools::errorf("Syntax-Error: missing end\n");
            return false;
        }

        return true;
    }
}
