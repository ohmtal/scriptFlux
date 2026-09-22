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

    std::shared_ptr<BlockStatement> ParseScriptToAST(std::istream& stream) {
        std::string line;
        int lineCount = 0;

        auto mainProgram = std::make_shared<BlockStatement>();

        std::vector<OpenBlock> blockStack;
        blockStack.push_back({BlockType::IfBlock, 0, mainProgram.get()});

        while (std::getline(stream, line)) {
            lineCount++;

            Globals::currentScriptLineNumber = lineCount;

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

            for (auto& ast : statements) {
                if (!ast) continue;

                // --- fn ---
                if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
                    blockStack.push_back({BlockType::Function, startNode->mFnNameSymbolId, nullptr});
                    continue;
                }

                bool isIf = dynamic_cast<IfStatement*>(ast.get()) != nullptr;
                bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
                bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;

                if (isFor || isWhile || isIf) {
                    std::shared_ptr<ASTNode> sharedBase = std::move(ast);
                    std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(sharedBase);

                    BlockType bType = isIf ? BlockType::IfBlock : (isFor ? BlockType::ForLoop : BlockType::WhileLoop);
                    BlockStatement* blockPtr = sharedLoop.get();

                    auto& outerBlock = blockStack.back();
                    if (outerBlock.mType == BlockType::Function) {
                        FunctionMap::RegisteredScriptFunctions[outerBlock.mFuncNameSymbolId].body.push_back(sharedLoop);
                    } else if (outerBlock.mType == BlockType::ForLoop || outerBlock.mType == BlockType::WhileLoop || outerBlock.mType == BlockType::IfBlock) {
                        outerBlock.mBlockNodePointer->mBody.push_back(sharedLoop);
                    }

                    blockStack.push_back({bType, 0, blockPtr});
                    continue;
                }

                if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
                    if (blockStack.size() <= 1) {
                        Tools::PrintParseError("Syntax-Error: 'end' without starting statement.");
                        return nullptr;
                    }
                    blockStack.pop_back();
                    continue;
                }

                // --- else ---
                if (dynamic_cast<ElseMarkerNode*>(ast.get())) {
                    if (blockStack.empty() || blockStack.back().mType != BlockType::IfBlock) {
                        Tools::PrintParseError("Syntax-Error: 'else' without matching 'if'.");
                        return nullptr;
                    }
                    auto* actualIf = dynamic_cast<IfStatement*>(blockStack.back().mBlockNodePointer);
                    if (actualIf) actualIf->mIsInElseBranch = true;
                    continue;
                }


                auto& currentBlock = blockStack.back();
                std::shared_ptr<ASTNode> sharedAst = std::move(ast);

                if (currentBlock.mType == BlockType::Function) {
                    FunctionMap::RegisteredScriptFunctions[currentBlock.mFuncNameSymbolId].body.push_back(sharedAst);
                } else {
                    auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.mBlockNodePointer);
                    if (currentBlock.mType == BlockType::IfBlock && actualIf && actualIf->mIsInElseBranch) {
                        actualIf->mElseBody.push_back(sharedAst);
                    } else {
                        currentBlock.mBlockNodePointer->mBody.push_back(sharedAst);
                    }
                }
            }
        }

        if (blockStack.size() > 1) {
            Tools::PrintParseError("Syntax-Error: missing end");
            return nullptr;
        }

        return mainProgram;
    }

    bool RunScriptStream(std::istream& stream, Environment& env) {

        auto mainProgram = ParseScriptToAST(stream);
        if (!mainProgram) return false;

        env.execute(mainProgram.get());

        return true;
    }



    // // bool RunScriptStream(std::istream& stream, Environment& env) {
    // //     if (!stream) {
    // //         Tools::errorf("Script stream is not in a valid state.\n");
    // //         return false;
    // //     }
    // //
    // //     std::string line;
    // //     std::vector<OpenBlock> blockStack;
    // //     std::unordered_map<BlockStatement*, std::shared_ptr<BlockStatement>> globalLoopKeeper;
    // //
    // //     int lineCount = 0;
    // //     bool doOutBreak = false;
    // //
    // //
    // //     while (std::getline(stream, line) && !doOutBreak) {
    // //         lineCount++;
    // //         Globals::currentScriptLineNumber = lineCount;
    // //
    // //         size_t firstRealChar = line.find_first_not_of(" \t\r\n");
    // //         if (firstRealChar == std::string::npos) continue;
    // //
    // //         // shell script style
    // //         if (line[firstRealChar] == '#') continue;
    // //
    // //         // lua style - because lua Syntax highlight is ok for DreiZehn ;)
    // //         if (line[firstRealChar] == '-' &&
    // //             firstRealChar + 1 < line.length() &&
    // //             line[firstRealChar + 1] == '-') {
    // //             continue;
    // //         }
    // //
    // //         Globals::currentScriptLine = line;
    // //         Lexer lexer(line);
    // //         auto tokens = lexer.tokenize();
    // //         Parser parser(tokens);
    // //
    // //         auto statements = parser.parseStatements();
    // //
    // //         // --------------------------------
    // //         // Run LineStatements
    // //         // --------------------------------
    // //
    // //         for (auto& ast : statements) {
    // //         // for (auto& ast : allStatements) {
    // //             if (!ast) continue;
    // //
    // //             // --- fn ---
    // //             if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
    // //                 blockStack.push_back({BlockType::Function, startNode->mFnNameSymbolId, nullptr});
    // //                 continue;
    // //             }
    // //
    // //             bool isIf = dynamic_cast<IfStatement*>(ast.get()) != nullptr;
    // //             bool isFor = dynamic_cast<ForStatement*>(ast.get()) != nullptr;
    // //             bool isWhile = dynamic_cast<WhileStatement*>(ast.get()) != nullptr;
    // //
    // //             if (isFor || isWhile || isIf) {
    // //                 std::shared_ptr<ASTNode> sharedBase = std::move(ast);
    // //                 std::shared_ptr<BlockStatement> sharedLoop = std::static_pointer_cast<BlockStatement>(sharedBase);
    // //
    // //                 BlockType bType;
    // //                 BlockStatement* blockPtr = nullptr;
    // //                 if (isIf) {
    // //                     bType = BlockType::IfBlock;
    // //                     blockPtr = dynamic_cast<BlockStatement*>(sharedBase.get());
    // //                 } else {
    // //                     bType = isFor ? BlockType::ForLoop : BlockType::WhileLoop;
    // //                     blockPtr = std::static_pointer_cast<BlockStatement>(sharedBase).get();
    // //                 }
    // //
    // //                 blockStack.push_back({bType, 0, blockPtr});
    // //                 // ------
    // //                 if (blockStack.size() > 1) {
    // //                     auto& outerBlock = blockStack[blockStack.size() - 2];
    // //                     if (outerBlock.mType == BlockType::Function) {
    // //                         FunctionMap::RegisteredScriptFunctions[outerBlock.mFuncNameSymbolId].body.push_back(sharedLoop);
    // //                     }
    // //                     else if (outerBlock.mType == BlockType::ForLoop) {
    // //                         auto* actualFor = dynamic_cast<ForStatement*>(outerBlock.mBlockNodePointer);
    // //                         if (actualFor) {
    // //                             actualFor->mBody.push_back(sharedLoop);
    // //                         }
    // //                     }
    // //                     else if (outerBlock.mType == BlockType::WhileLoop) {
    // //                         auto* actualWhile = dynamic_cast<WhileStatement*>(outerBlock.mBlockNodePointer);
    // //                         if (actualWhile) {
    // //                             actualWhile->mBody.push_back(sharedLoop);
    // //                         }
    // //                     }
    // //                     else if (outerBlock.mType == BlockType::IfBlock) {
    // //                         auto* actualIf = dynamic_cast<IfStatement*>(outerBlock.mBlockNodePointer);
    // //                         if (actualIf) actualIf->mBody.push_back(sharedBase);
    // //                     }
    // //                 } else {
    // //                     // IMPORTANT: keep the "shared_ptr" pointer alive:
    // //                     globalLoopKeeper[sharedLoop.get()] = sharedLoop;
    // //                 }
    // //                 continue;
    // //             }
    // //
    // //
    // //
    // //             if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
    // //                 if (blockStack.empty()) {
    // //                     Tools::PrintParseError("Syntax-Error: 'end' without starting statement.");
    // //                     return false;
    // //                 }
    // //
    // //                 OpenBlock closingBlock = blockStack.back();
    // //                 blockStack.pop_back();
    // //
    // //                 if ((closingBlock.mType == BlockType::ForLoop
    // //                     || closingBlock.mType == BlockType::WhileLoop
    // //                     || closingBlock.mType == BlockType::IfBlock)
    // //                     && blockStack.empty()) {
    // //                     env.execute(closingBlock.mBlockNodePointer);
    // //                 }
    // //                 continue;
    // //             }
    // //             // --- else ---
    // //             if (dynamic_cast<ElseMarkerNode*>(ast.get())) {
    // //                 if (blockStack.empty() || blockStack.back().mType != BlockType::IfBlock) {
    // //                     Tools::PrintParseError("Syntax-Error: 'else' without matching 'if'.");
    // //                     return false;
    // //                 }
    // //
    // //                 auto& currentBlock = blockStack.back();
    // //                 auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.mBlockNodePointer);
    // //                 if (actualIf) {
    // //                     // rewrite
    // //                     actualIf->mIsInElseBranch = true;
    // //                 }
    // //                 continue;
    // //             }
    // //             // --- command ---
    // //             if (!blockStack.empty()) {
    // //                 auto& currentBlock = blockStack.back();
    // //                 std::shared_ptr<ASTNode> sharedAst = std::move(ast);
    // //
    // //                 if (currentBlock.mType == BlockType::Function) {
    // //                     FunctionMap::RegisteredScriptFunctions[currentBlock.mFuncNameSymbolId].body.push_back(sharedAst);
    // //                 } else if (currentBlock.mType == BlockType::ForLoop || currentBlock.mType == BlockType::WhileLoop) {
    // //                     currentBlock.mBlockNodePointer->mBody.push_back(sharedAst);
    // //                 } else if (currentBlock.mType == BlockType::IfBlock) {
    // //                     auto* actualIf = dynamic_cast<IfStatement*>(currentBlock.mBlockNodePointer);
    // //                     if (actualIf) {
    // //                         if (actualIf->mIsInElseBranch) {
    // //                             actualIf->mElseBody.push_back(sharedAst);
    // //                         } else {
    // //                             actualIf->mBody.push_back(sharedAst);
    // //                         }
    // //                     }
    // //                 }
    // //
    // //             } else {
    // //                 FlowSignal sig = env.execute(ast.get());
    // //                 if (sig == FlowSignal::Return) {
    // //                     doOutBreak = true;
    // //                     break;
    // //                 }
    // //             }
    // //         } // end of statements
    // //     } // end of while
    // //
    // //
    // //     if (!blockStack.empty()) {
    // //         Tools::PrintParseError("Syntax-Error: missing end");
    // //         return false;
    // //     }
    // //
    // //     return true;
    // // }
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
