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

        while (std::getline(stream, line)) {
            lineCount++;

            size_t firstRealChar = line.find_first_not_of(" \t\r\n");
            if (firstRealChar == std::string::npos) continue;
            if (line[firstRealChar] == '#') continue;

            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);

            auto statements = parser.parseStatements();

            for (auto& ast : statements) {
                if (!ast) continue;

                // --- fn ---
                if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
                    blockStack.push_back({BlockType::Function, startNode->name, nullptr});
                    continue;
                }

                bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
                bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;

                if (isFor || isWhile) {
                    std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                    std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(sharedBase);

                    BlockType bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
                    blockStack.push_back({bType, "", sharedLoop.get()});

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
                    } else {
                        globalLoopKeeper[sharedLoop.get()] = sharedLoop;
                    }
                    continue;
                }


                // bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
                // bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;
                //
                // if (isFor || isWhile) {
                //     std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(ast);
                //
                //     ast.reset();
                //
                //     BlockType bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
                //     blockStack.push_back({bType, "", sharedLoop.get()});
                //
                //     if (blockStack.size() > 1) {
                //         auto& outerBlock = blockStack[blockStack.size() - 2];
                //         if (outerBlock.type == BlockType::Function) {
                //             FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedLoop);
                //         } else if (outerBlock.type == BlockType::ForLoop || outerBlock.type == BlockType::WhileLoop) {
                //             outerBlock.blockNodePointer->body.push_back(sharedLoop);
                //         }
                //     } else {
                //         globalLoopKeeper[sharedLoop.get()] = sharedLoop;
                //     }
                //     continue;
                // }



                // // bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
                // // bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;
                // //
                // // if (isFor || isWhile) {
                // //     std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                // //     std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(sharedBase);
                // //
                // //     BlockType bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
                // //     blockStack.push_back({bType, "", sharedLoop.get()});
                // //
                // //     if (blockStack.size() > 1) {
                // //         auto& outerBlock = blockStack[blockStack.size() - 2];
                // //         if (outerBlock.type == BlockType::Function) {
                // //             FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedLoop);
                // //         } else if (outerBlock.type == BlockType::ForLoop || outerBlock.type == BlockType::WhileLoop) {
                // //             outerBlock.blockNodePointer->body.push_back(sharedLoop);
                // //         }
                // //     } else {
                // //         globalLoopKeeper[sharedLoop.get()] = sharedLoop;
                // //     }
                // //     continue;
                // // }

                // // // --- for ---
                // // if (dynamic_cast<ForStatement*>(ast.get())) {
                // //     std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                // //     std::shared_ptr<ForStatement> sharedFor = std::static_pointer_cast<ForStatement>(sharedBase);
                // //
                // //     blockStack.push_back({BlockType::ForLoop, "", sharedFor.get()});
                // //
                // //     if (blockStack.size() > 1) {
                // //         auto& outerBlock = blockStack[blockStack.size() - 2];
                // //         if (outerBlock.type == BlockType::Function) {
                // //             FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedFor);
                // //         } else if (outerBlock.type == BlockType::ForLoop) {
                // //             outerBlock.forNodePointer->body.push_back(sharedFor);
                // //         }
                // //     } else {
                // //         globalForKeeper[sharedFor.get()] = sharedFor;
                // //     }
                // //     continue;
                // // }
                // // // --- while ---
                // // if (dynamic_cast<WhileStatement*>(ast.get())) {
                // //     std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                // //     std::shared_ptr<WhileStatement> sharedWhile = std::static_pointer_cast<WhileStatement>(sharedBase);
                // //
                // //     blockStack.push_back({BlockType::WhileLoop, "", nullptr, sharedWhile.get()});
                // //
                // //     if (blockStack.size() > 1) {
                // //         auto& outerBlock = blockStack[blockStack.size() - 2];
                // //         if (outerBlock.type == BlockType::Function) {
                // //             FunctionMap::RegisteredScriptFunctions[outerBlock.funcName].body.push_back(sharedWhile);
                // //         } else if (outerBlock.type == BlockType::WhileLoop) {
                // //             outerBlock.forNodePointer->body.push_back(sharedWhile);
                // //         }
                // //     } else {
                // //         globalWhileKeeper[sharedWhile.get()] = sharedWhile;
                // //     }
                // //     continue;
                // // }
                // ---  end ---
                if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
                    if (blockStack.empty()) {
                        Tools::errorf("[Line %d] Syntax-Error: 'end' without if/for.\n", lineCount);
                        return false;
                    }

                    OpenBlock closingBlock = blockStack.back();
                    blockStack.pop_back();

                    if ((closingBlock.type == BlockType::ForLoop || closingBlock.type == BlockType::WhileLoop)
                        && blockStack.empty()) {
                        env.execute(closingBlock.blockNodePointer);
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
                    }
                } else {
                    env.execute(ast.get());
                }
            } // end of statements
        } // end of while


        if (!blockStack.empty()) {
            Tools::errorf("Syntax-Error: missing end\n");
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
