#include <vector>
#include <iostream>

#include "DreiZehn.h"


int main(int argc, char* argv[]) {
    using namespace DreiZehn;

    Environment env;
    RegisterCoreFunctions(env);

    if (argc > 1) {
        std::string scriptPath = argv[1];

        bool success = RunScriptFile(scriptPath, env);

        env.shutDown();

        return success ? 0 : 1;
    }

    // -------------------------------------------------------------------------
    // Console Mode
    // -------------------------------------------------------------------------
    std::string line;

    std::vector<OpenBlock> blockStack;

    while (true) {
        for (size_t i = 0; i < blockStack.size(); ++i) std::cout << ".. ";
        std::cout << (blockStack.empty() ? "> " : "");

        std::getline(std::cin, line);
        if (line == "exit") break;

        Lexer lexer(line);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto ast = parser.parseLine();

        if (!ast) continue;

        if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
            blockStack.push_back({BlockType::Function, startNode->name, nullptr});
            Tools::printf("compile function '%s'...\n", startNode->name.c_str());
            continue;
        }

        if (auto* forNode = dynamic_cast<ForStatement*>(ast.get())) {
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
                static std::unordered_map<ForStatement*, std::shared_ptr<ForStatement>> globalForKeeper;
                globalForKeeper[sharedFor.get()] = sharedFor;
            }
            continue;
        }

        if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
            if (blockStack.empty()) {
                Tools::errorf("Syntax-Error: 'end' without for / fn.\n");
                continue;
            }

            OpenBlock closingBlock = blockStack.back();
            blockStack.pop_back();

            if (closingBlock.type == BlockType::Function) {
                Tools::printf("function '%s' registered.\n", closingBlock.funcName.c_str());
            }
            else if (closingBlock.type == BlockType::ForLoop) {
                if (blockStack.empty()) {
                    env.execute(closingBlock.forNodePointer);
                }
            }
            continue;
        }

        if (!blockStack.empty()) {
            auto& currentBlock = blockStack.back();
            std::shared_ptr<ASTNode> sharedAst = std::move(ast);

            if (currentBlock.type == BlockType::Function) {
                FunctionMap::RegisteredScriptFunctions[currentBlock.funcName].body.push_back(sharedAst);
            }
            else if (currentBlock.type == BlockType::ForLoop) {
                currentBlock.forNodePointer->body.push_back(sharedAst);
            }
        }
        else {
            env.execute(ast.get());
        }
    }
    env.shutDown();
    return 0;
}



// int main() {
//
//     using namespace DreiZehn;
//
//     Environment env;
//     RegisterCoreFunctions(env);
//
//     std::string line;
//     bool isRecordingFunction = false;
//     std::string currentRecordingFuncName = "";
//
//     Tools::printf("DreiZehn CLI ready.\n");
//     while (true) {
//         Tools::printf (isRecordingFunction ? ".. " : "> ");
//         std::getline(std::cin, line);
//         if (line == "exit") break;
//
//         // 1. Lexer
//         Lexer lexer(line);
//         auto tokens = lexer.tokenize();
//
//         // 2. Parser
//         Parser parser(tokens);
//         auto ast = parser.parseLine();
//
//         if (!ast) continue;
//
//         //NOTE  special handline for fn input!!!
//         if (auto* startNode = dynamic_cast<FunctionDefineStartNode*>(ast.get())) {
//             isRecordingFunction = true;
//             currentRecordingFuncName = startNode->name;
//             Tools::printf("Compile function: '%s'...\n", currentRecordingFuncName.c_str());
//             continue;
//         }
//
//         if (dynamic_cast<FunctionDefineEndNode*>(ast.get())) {
//             Tools::printf("function '%s' registered.\n", currentRecordingFuncName.c_str());
//             isRecordingFunction = false;
//             currentRecordingFuncName = "";
//             continue;
//         }
//
//         if (isRecordingFunction) {
//             auto& func = FunctionMap::RegisteredScriptFunctions[currentRecordingFuncName];
//             func.body.push_back(std::move(ast));
//         }
//         else {
//             env.execute(ast.get());
//         }
//     }
//     env.shutDown();
//     return 0;
// }
//
//
