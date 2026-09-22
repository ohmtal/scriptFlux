//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Loader
//-----------------------------------------------------------------------------
#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include "Environment.h"
#include "Tools.h"
#include "Parser.h"

namespace DreiZehn {


    bool RunScriptStream(std::istream& stream, Environment& env) {
        if (!stream) {
            Tools::errorf("Script stream is not in a valid state.\n");
            return false;
        }

        std::string line;
        std::vector<OpenBlock> blockStack;
        std::unordered_map<BlockStatement*, std::shared_ptr<BlockStatement>> globalLoopKeeper;

        int lineCount = 0;
        bool doOutBreak = false;

        // std::vector<std::unique_ptr<ASTNode>> allStatements;


        while (std::getline(stream, line) && !doOutBreak) {
            lineCount++;
            Globals::currentScriptLine = lineCount;

            size_t firstRealChar = line.find_first_not_of(" \t\r\n");
            if (firstRealChar == std::string::npos) continue;

            // shell script style
            if (line[firstRealChar] == '#') continue;




            // lua style - because lua Syntax highlight is ok for DreiZehn ;)
            if (line[firstRealChar] == '-' &&
                firstRealChar + 1 < line.length() &&
                line[firstRealChar + 1] == '-') {
                continue;
            }

            Globals::currentScriptLine = line;
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);

            auto statements = parser.parseStatements();

            // NOT faster! std::ranges::move(statements, std::back_inserter(allStatements));
            // but maybe usful to store the prog ? .. but for what
        // };


            for (auto& ast : statements) {
            // for (auto& ast : allStatements) {
                if (!ast) continue;

                // --- fn ---
                if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
                    blockStack.push_back({BlockType::Function, startNode->name, nullptr});
                    continue;
                }

                bool isIf = dynamic_cast<IfStatement*>(ast.get()) != nullptr;
                bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
                bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;

                if (isFor || isWhile || isIf) {
                    std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                    std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(sharedBase);

                    // BlockType bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
                    // blockStack.push_back({bType, "", sharedLoop.get()});
                    BlockType bType;
                    BlockStatement* blockPtr = nullptr; // Oder Basis-Typ, von dem deine Stmts erben

                    if (isIf) {
                        bType = BlockType::IfBlock;
                        blockPtr = dynamic_cast<BlockStatement*>(sharedBase.get());
                    } else {
                        bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
                        blockPtr = std::static_pointer_cast<BlockStatement>(sharedBase).get();
                    }

                    blockStack.push_back({bType, "", blockPtr});
                    // ------
                    if (blockStack.size() > 1) {
                        auto& outerBlock = blockStack[blockStack.size() - 2];
                        if (outerBlock.type == BlockType::Function) {
                            FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedLoop);
                        }
                        else if (outerBlock.type == BlockType::ForLoop) {
                            auto* actualFor = dynamic_cast<ForStatement*>(outerBlock.blockNodePointer);
                            if (actualFor) {
                                actualFor->body.push_back(sharedLoop);
                            }
                        }
                        else if (outerBlock.type == BlockType::WhileLoop) {
                            auto* actualWhile = dynamic_cast<WhileStatement*>(outerBlock.blockNodePointer);
                            if (actualWhile) {
                                actualWhile->body.push_back(sharedLoop);
                            }
                        }
                        else if (outerBlock.type == BlockType::IfBlock) {
                            auto* actualIf = dynamic_cast<IfStatement*>(outerBlock.blockNodePointer);
                            if (actualIf) actualIf->body.push_back(sharedBase);
                        }
                    } else {
                        // IMPORTANT: keep the "shared_ptr" pointer alive:
                        globalLoopKeeper[sharedLoop.get()] = sharedLoop;
                    }
                    continue;
                }



                if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
                    if (blockStack.empty()) {
                        Tools::errorf("[Line %d] Syntax-Error: 'end' without starting statement.\n", Globals::currentScriptLine);
                        return false;
                    }

                    OpenBlock closingBlock = blockStack.back();
                    blockStack.pop_back();

                    if ((closingBlock.type == BlockType::ForLoop
                        || closingBlock.type == BlockType::WhileLoop
                        || closingBlock.type == BlockType::IfBlock)
                        && blockStack.empty()) {
                        env.execute(closingBlock.blockNodePointer);
                    }
                    continue;
                }
                // --- else ---
                if (dynamic_cast<ElseMarkerNode*>(ast.get())) {
                    if (blockStack.empty() || blockStack.back().type != BlockType::IfBlock) {
                        Tools::errorf("[Line %d] Syntax-Error: 'else' without matching 'if'.\n", lineCount);
                        return false;
                    }

                    auto& currentBlock = blockStack.back();
                    auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.blockNodePointer);
                    if (actualIf) {
                        // rewrite
                        actualIf->mIsInElseBranch = true;
                    }
                    continue;
                }
                // --- command ---
                if (!blockStack.empty()) {
                    auto& currentBlock = blockStack.back();
                    std::shared_ptr<ASTNode> sharedAst = std::move(ast);

                    if (currentBlock.type == BlockType::Function) {
                        FunctionMap::RegisteredScriptFunctions[currentBlock.funcName].body.push_back(sharedAst);
                    } else if (currentBlock.type == BlockType::ForLoop || currentBlock.type == BlockType::WhileLoop) {
                        currentBlock.blockNodePointer->body.push_back(sharedAst);
                    } else if (currentBlock.type == BlockType::IfBlock) {
                        // pre else:
                        // auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.blockNodePointer);
                        // if (actualIf) actualIf->body.push_back(sharedAst);
                        auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.blockNodePointer);
                        if (actualIf) {
                            if (actualIf->mIsInElseBranch) {
                                actualIf->elseBody.push_back(sharedAst);
                            } else {
                                actualIf->body.push_back(sharedAst);
                            }
                        }
                    }

                } else {
                    // DEBUG
                    FlowSignal sig = env.execute(ast.get());
                    if (sig == FlowSignal::Return) {
                        doOutBreak = true;
                        break;
                    }
                }
            } // end of statements
        } // end of while


        if (!blockStack.empty()) {
            Tools::PrintParseError("Syntax-Error: missing end");
            return false;
        }

        return true;
    }
    // --------------------------------------------------------------------------------
    bool RunScriptFile(const std::string& filename, Environment& env) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Tools::errorf("Script file cant be opened: %s\n", filename.c_str());
            return false;
        }
        return RunScriptStream(file, env);
    }
}
