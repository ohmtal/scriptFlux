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

namespace DreiZehn {

    enum class BlockType { Function, ForLoop, WhileLoop, IfBlock };

    struct OpenBlock {
        BlockType type;
        std::string funcName;
        BlockStatement* blockNodePointer;
    };


    enum class FlowSignal {
        None,
        Break,
        Return
    };



class Environment {
private:
    // variables stack
    std::unordered_map<std::string, Value> variables;

    // Garbage collection
    std::vector<ValueObject*> mGarbageCollection;
    Environment* parent = nullptr;
public:
    Environment() : parent(nullptr) {
        Globals::gCurEnv = this;
    }
    Environment(Environment* parentEnv) : parent(parentEnv) {
        Globals::gCurEnv = this;
    }
    ~Environment() {
        doGarbageCollection();
        if (parent) Globals::gCurEnv = parent;
    }

    // -------------------------------------------------------------------------

    void setVariable(const std::string& name, Value val) {

        auto it = variables.find(name);
        if (it != variables.end()) {
            it->second = val;
        }
        // if (variables.find(name) != variables.end()) {
        //     variables[name] = val;
        //     return;
        // }

        if (parent != nullptr) {
            parent->setVariable(name, val);
            return;
        }

        variables[name] = val;
    }



    Value getVariable(const std::string& name) {
        if (variables.find(name) != variables.end()) {
            return variables.at(name);
        }
        if (parent != nullptr) {
            return parent->getVariable(name); // global scope
        }
        Tools::errorf("Variable not found: %s\n", name.c_str());
        return Value();
    }
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

        // --- Break Statement ---
        if (dynamic_cast<BreakStatement*>(node)) {
            return FlowSignal::Break;
        }

        // --- Return Statement ---
        if (auto* retStmt = dynamic_cast<ReturnStatement*>(node)) {
            if (retStmt->expression) {
                Value retVal = retStmt->expression->evaluate(currentEnv);
                currentEnv.setVariable("__return_value__", retVal);
            }
            return FlowSignal::Return;
        }

        // --- Assign ---
        if (auto* assign = dynamic_cast<AssignStatement*>(node)) {
            currentEnv.setVariable(assign->varName, assign->rhs->evaluate(currentEnv));
        }
        // --- If-Statement  ---
        else if (auto* ifStmt = dynamic_cast<IfStatement*>(node)) {
            Value condVal = ifStmt->condition->evaluate(currentEnv);

            double condNum = condVal.getDouble();
            const double EPSILON = 1e-9;
            bool isTrue = std::abs(condNum) > EPSILON;

            if (isTrue) {
                for (auto& childNode : ifStmt->body) {
                    if (!childNode) continue;
                    FlowSignal sig = execute(childNode.get(), currentEnv);
                    if (sig != FlowSignal::None) return sig;
                }
            } else {
                for (auto& childNode : ifStmt->elseBody) {
                    if (!childNode) continue;
                    FlowSignal sig = execute(childNode.get(), currentEnv);
                    if (sig != FlowSignal::None) return sig;
                }
            }
        }


        // else if (auto* ifStmt = dynamic_cast<IfStatement*>(node)) {
        //     Value condVal = ifStmt->condition->evaluate(currentEnv);
        //     bool isTrue = (condVal.isInt() && condVal.asInt() != 0) || (condVal.isDouble() && condVal.asDouble() != 0.0);
        //
        //     if (isTrue) {
        //         FlowSignal sig = execute(ifStmt->thenBranch.get(), currentEnv);
        //         if (sig != FlowSignal::None) return sig;
        //     }
        // }
        // ---- for statement .....
        else if (auto* forStmt = dynamic_cast<ForStatement*>(node)) {
            Value startVal = forStmt->startExpr->evaluate(currentEnv);
            Value endVal = forStmt->endExpr->evaluate(currentEnv);

            if (!startVal.isInt() || !endVal.isInt()) {
                Tools::errorf("Error: 'for'-loop only support integer borders\n");
                return FlowSignal::None;
            }

            int start = startVal.asInt();
            int end = endVal.asInt();

            Environment loopEnv(&currentEnv);

            for (int i = start; i <= end; ++i) {
                loopEnv.setVariable(forStmt->iteratorName, Value(i));

                for (auto& statement : forStmt->body) {
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
                for (auto& statement : whileStmt->body) {
                    FlowSignal sig = currentEnv.execute(statement.get(), loopEnv);

                    if (sig == FlowSignal::Break) return FlowSignal::None;
                    if (sig == FlowSignal::Return) return FlowSignal::Return;
                }
            }
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
