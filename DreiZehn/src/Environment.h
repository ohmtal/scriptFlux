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

namespace DreiZehn {

using CallBack =  std::function< bool (std::vector<Value>&, Value& )>;
using FuncLookupMap = std::unordered_map<std::string, CallBack>;

class Environment {
private:

    // variables stack
    std::unordered_map<std::string, Value> mVariables;

    // Garbage collection
    std::vector<ValueObject*> mGarbageCollection;

    // Functions
    FuncLookupMap registeredFunctions;

public:
    // -------------------------------------------------------------------------
    // Function Registry
    // -------------------------------------------------------------------------
    void registerFunction(const std::string& name, CallBack cb) {
        registeredFunctions[name] = cb;
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
    // resolve the token to value

    Value resolve(const std::string& tokenStr) {
        // a String-Literal ?
        if (tokenStr.size() >= 2 && tokenStr.front() == '"' && tokenStr.back() == '"') {
            // clean string again
            std::string cleanStr = tokenStr.substr(1, tokenStr.size() - 2);
            StringValueObject* strObj = new StringValueObject(cleanStr);
            addToGarbageCollection(strObj);
            return Value(strObj);
        }

        // Variable?
        if (mVariables.find(tokenStr) != mVariables.end()) {
            return mVariables.at(tokenStr);
        }

        // Number
        if (tokenStr.find('.') != std::string::npos) {
            return Value(std::stod(tokenStr));
        }
        // INT
        if (!tokenStr.empty() && std::all_of(tokenStr.begin(), tokenStr.end(), ::isdigit)) {
            return Value(std::stoi(tokenStr));
        }

        // Fallback: Error
        Tools::errorf("[Runtime-Error] Unknown Symbol: %s\n", tokenStr.c_str());
        return Value();
    }

    // -------------------------------------------------------------------------
    // Here we go ...
    void execute(ASTNode* node) {
        if (!node) return;

        // Assign
        if (auto* assign = dynamic_cast<AssignNode*>(node)) {
            mVariables[assign->varName] = resolve(assign->valueStr);
            //FIXME DEBUG STUFF std::cout << "[Runtime] Variable '" << assign->varName << "' set.\n";
        }

        else
        // Command
            if (auto* cmd = dynamic_cast<CommandNode*>(node)) {
                // Args
                std::vector<Value> evalArgs;
                for (const auto& argStr : cmd->args) {
                    evalArgs.push_back(resolve(argStr));
                }

                // function lookup
                if (registeredFunctions.find(cmd->cmdName) != registeredFunctions.end()) {
                    Value returnValue;
                    bool success = registeredFunctions[cmd->cmdName](evalArgs, returnValue);

                    if (!success) {
                        Tools::errorf("Command error at: %s\n", cmd->cmdName.c_str());
                    }

                    //TODO use the result of the return value
                }
                else {
                    Tools::errorf("Unknown Command: %s\n", cmd->cmdName.c_str());
                }
            }

    } // execute
    // -------------------------------------------------------------------------
    void shutDown() {
        doGarbageCollection();
    }
}; //Class
} //Namespace
