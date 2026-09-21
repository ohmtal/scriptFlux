//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Function Map
//-----------------------------------------------------------------------------
#pragma once

#include <functional>
#include <vector>
#include <string>
#include "Value.h"

namespace DreiZehn::FunctionMap {
    class Enviorment;
    using CallBack =  std::function< bool ( std::vector<Value>&, Value& )>;
    using FuncLookupMap = std::unordered_map<std::string, CallBack>;

    // Functions
    inline FuncLookupMap RegisteredFunctions;

    inline void RegisterFunction(const std::string& name, CallBack cb) {
        RegisteredFunctions[name] = cb;
    }

    inline bool IsCFunction (const std::string& name) {
        return RegisteredFunctions.find(name) != RegisteredFunctions.end();
    }

    // --------------- SCRIPT FUNCTION ----------------------

    struct ScriptFunction {
        std::vector<std::string> parameterNames;
        std::vector<std::shared_ptr<ASTNode>> body;
    };

    inline std::unordered_map<std::string, ScriptFunction> RegisteredScriptFunctions;

    inline bool IsScriptFunction(const std::string& name) {
        return RegisteredScriptFunctions.find(name) != RegisteredScriptFunctions.end();
    }

    // -------------------------------------------------------------
    // combined for parser
    inline bool IsFunction (const std::string& name) {
        return IsCFunction(name) || IsScriptFunction(name);
    }
    // -------------------------------------------------------------

} //namespace


