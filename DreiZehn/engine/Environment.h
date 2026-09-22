//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Enviroment VM
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

#include <unordered_map>
#include <iostream>
#include <functional>

#include "Value.h"
#include "AstNode.h"
#include "Tools.h"
#include "FunctionMap.h"
#include "Globals.h"
#include "SymbolTable.h"

namespace DreiZehn {

    enum class BlockType { Function, ForLoop, WhileLoop, IfBlock };

    struct OpenBlock {
        BlockType mType;
        uint32_t mFuncNameSymbolId;
        BlockStatement* mBlockNodePointer;
    };


    enum class FlowSignal {
        None,
        Break,
        Return
    };



class Environment {
private:
    // variables stack
    // std::unordered_map<std::string, Value> mVariables;
    std::unordered_map<uint32_t, Value> mVariables;

    // Garbage collection
    std::vector<ValueObject*> mGarbageCollection;
    Environment* mParentEnv = nullptr;
public:
    Environment() : mParentEnv(nullptr) {
        Globals::gCurEnv = this;
    }
    Environment(Environment* parentEnv) : mParentEnv(parentEnv) {
        Globals::gCurEnv = this;
    }
    ~Environment() {
        doGarbageCollection();
        if (mParentEnv) Globals::gCurEnv = mParentEnv;
    }

    // -------------------------------------------------------------------------
    // Variable getter/setter
    // -------------------------------------------------------------------------
    void setVariable(uint32_t id, Value val) {

        if (Globals::gShowVariableDebug) Tools::printf("DEBUG: setVariable :: name: %s id: %d, floatval: %f\n", SymbolTable::getName(id).c_str(), id, val.getFloat());

        auto it = mVariables.find(id);
        if (it != mVariables.end()) {
            it->second = val;
            return;
        }

        if (mParentEnv != nullptr) {
            if (mParentEnv->tryUpdateVariable(id, val)) {
                return;
            }
        }
        mVariables[id] = val;
    }
    // -------------------------------------------------------------------------
    bool tryUpdateVariable(uint32_t id, Value val) {
        auto it = mVariables.find(id);
        if (it != mVariables.end()) {
            it->second = val;
            return true;
        }
        if (mParentEnv != nullptr) {
            return mParentEnv->tryUpdateVariable(id, val);
        }
        return false;
    }
    // // -------------------------------------------------------------------------
    Value getVariable(uint32_t id) {
        if (Globals::gShowVariableDebug) Tools::printf("DEBUG: getVariable :: name: %s id: %d\n", SymbolTable::getName(id).c_str(), id);

        auto it = mVariables.find(id);
        if (it != mVariables.end()) {
            return it->second;
        }

        if (mParentEnv != nullptr) {
            return mParentEnv->getVariable(id);
        }

        std::string varName = SymbolTable::getName(id);
        Tools::errorf("Variable not found: %s\n", varName.c_str());
        return Value();
    }


// //     void setVariable(const std::string& name, Value val) {
// //
// //         auto it = mVariables.find(name);
// //         if (it != mVariables.end()) {
// //             it->second = val;
// //         }
// //         if (mParentEnv != nullptr) {
// //             mParentEnv->setVariable(name, val);
// //             return;
// //         }
// //
// //         mVariables[name] = val;
// //     }
// //
// //     Value getVariable(const std::string& name) {
// //         if (mVariables.find(name) != mVariables.end()) {
// //             return mVariables.at(name);
// //         }
// //         if (mParentEnv != nullptr) {
// //             return mParentEnv->getVariable(name); // global scope
// //         }
// //         Tools::errorf("Variable not found: %s\n", name.c_str());
// //         return Value();
// //     }
    // -------------------------------------------------------------------------
    // GarbageCollection
    // -------------------------------------------------------------------------
    void addToGarbageCollection(ValueObject* obj) {
        mGarbageCollection.push_back(obj);
    }

    void doGarbageCollection() {
        for (auto* obj : mGarbageCollection) {
            delete obj;
        }
        mGarbageCollection.clear();
    }
    // -------------------------------------------------------------------------
    // EXECUTE :D - currentEnv for function calls
    // -------------------------------------------------------------------------
    inline FlowSignal execute(ASTNode* node, Environment& currentEnv) {
        if (!node) return FlowSignal::None;

        //NOTE HARDCORE DEBUG
        // std::cout << "DEBUG-EXECUTE: Node-Typ: " << typeid(*node).name() << "\n";


        // --- Break Statement ---
        if (dynamic_cast<BreakStatement*>(node)) {
            return FlowSignal::Break;
        }

        // --- Return Statement ---
        if (auto* retStmt = dynamic_cast<ReturnStatement*>(node)) {
            if (retStmt->mExpression) {
                Value retVal = retStmt->mExpression->evaluate(currentEnv);
                currentEnv.setVariable(SymbolTable::insert("__return_value__"), retVal);
            }
            return FlowSignal::Return;
        }

        // --- Assign ---
        if (auto* assign = dynamic_cast<AssignStatement*>(node)) {
            currentEnv.setVariable(SymbolTable::insert(assign->mVarName), assign->mRhs->evaluate(currentEnv));
        }
        // --- If-Statement  ---
        else if (auto* ifStmt = dynamic_cast<IfStatement*>(node)) {
            Value condVal = ifStmt->mCondition->evaluate(currentEnv);

            double condNum = condVal.getDouble();
            const double EPSILON = 1e-9;
            bool isTrue = std::abs(condNum) > EPSILON;

            if (isTrue) {
                for (auto& childNode : ifStmt->mBody) {
                    if (!childNode) continue;
                    FlowSignal sig = execute(childNode.get(), currentEnv);
                    if (sig != FlowSignal::None) return sig;
                }
            } else {
                for (auto& childNode : ifStmt->mElseBody) {
                    if (!childNode) continue;
                    FlowSignal sig = execute(childNode.get(), currentEnv);
                    if (sig != FlowSignal::None) return sig;
                }
            }
        }


        // ---- for statement .....
        else if (auto* forStmt = dynamic_cast<ForStatement*>(node)) {
            Value startVal = forStmt->mStartExpr->evaluate(currentEnv);
            Value endVal = forStmt->mEndExpr->evaluate(currentEnv);

            if (!startVal.isInt() || !endVal.isInt()) {
                Tools::errorf("Error: 'for'-loop only support integer borders\n");
                return FlowSignal::None;
            }

            int start = startVal.asInt();
            int end = endVal.asInt();

            Environment loopEnv(&currentEnv);

            for (int i = start; i <= end; ++i) {
                loopEnv.setVariable(forStmt->mIteratorVarNameSymbolId, Value(i));

                for (auto& statement : forStmt->mBody) {
                    FlowSignal sig = currentEnv.execute(statement.get(), loopEnv);

                    if (sig == FlowSignal::Break) {
                        return FlowSignal::None;
                    }
                    if (sig == FlowSignal::Return) {
                        return FlowSignal::Return;
                    }
                }
            }
        }
        // ---- While statement .....
        else if (auto* whileStmt = dynamic_cast<WhileStatement*>(node)) {
            Environment loopEnv(&currentEnv);

            auto checkCondition = [&]() -> bool {
                Value condVal = whileStmt->mCondition->evaluate(loopEnv);
                return (condVal.isInt() && condVal.asInt() != 0) ||
                (condVal.isDouble() && condVal.asDouble() != 0.0);
            };

            while (checkCondition()) {
                for (auto& statement : whileStmt->mBody) {
                    FlowSignal sig = currentEnv.execute(statement.get(), loopEnv);

                    if (sig == FlowSignal::Break) return FlowSignal::None;
                    if (sig == FlowSignal::Return) return FlowSignal::Return;
                }
            }
        }
        // --- BlockStatement  ---
        else
        if (auto* block = dynamic_cast<BlockStatement*>(node)) {
            for (auto& statement : block->mBody) {
                if (!statement) continue;
                FlowSignal sig = execute(statement.get(), *this);
                if (sig != FlowSignal::None) return sig;
            }
            return FlowSignal::None;
        }


        // --- others ---
        else if (auto* expr = dynamic_cast<Expression*>(node)) {
            expr->evaluate(currentEnv);
        }

        return FlowSignal::None;
    }


    // -------------------------------------------------------------------------
    // main execute
    inline FlowSignal execute(ASTNode* node) {
         return execute(node, *this);
    }
    // -------------------------------------------------------------------------
    void shutDown() {
        // done be destuctor: doGarbageCollection();
    }
}; //Class
} //Namespace
