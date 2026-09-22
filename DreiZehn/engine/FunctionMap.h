//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Function Map , TODO lookup by std::string is not the fastest!
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

    inline CallBack* GetCFunction (const std::string& name) {
        auto it = RegisteredFunctions.find(name);
        if (it != RegisteredFunctions.end())
            return &it->second;
        else
            return nullptr;
    }

    // ----------- Field validation
    // TODO .. i want to get forward with binding do this later!!
    // struct ParameterDef {
    //     ValueType fieldType = ValueType::Undefined;
    //     bool optional       = false;
    //     std::string docu    = "";
    // };
    // using FuncParameterList = std::vector<ParameterDef>;
    // using FuncParameterMap = std::unordered_map<std::string, FuncParameterList>;
    // inline FuncParameterMap RegisteredFuncParms;
    //
    // inline void RegisterFunctionParameter(const std::string& name, const FuncParameterList& funcParams) {
    //     RegisteredFuncParms[name] = funcParams;
    // }
    //
    // inline bool FunctionParamValidate(const std::string& name, const std::vector<Value>&) {
    //     // if (RegisteredFuncParms.din)
    // }
    //

    // --------------- SCRIPT FUNCTION ----------------------

    struct ScriptFunction {
        std::vector<std::string> parameterNames;
        std::vector<std::shared_ptr<ASTNode>> body;
    };

    inline std::unordered_map<std::string, ScriptFunction> RegisteredScriptFunctions;

    inline bool IsScriptFunction(const std::string& name) {
        return RegisteredScriptFunctions.find(name) != RegisteredScriptFunctions.end();
    }

    inline ScriptFunction* GetScriptFunction (const std::string& name) {
        auto it = RegisteredScriptFunctions.find(name);
        if (it != RegisteredScriptFunctions.end())
            return &it->second;
        else
            return nullptr;
    }

    // -------------------------------------------------------------
    // combined for parser
    inline bool IsFunction (const std::string& name) {
        return IsCFunction(name) || IsScriptFunction(name);
    }
    // -------------------------------------------------------------
    // Constants
    // -------------------------------------------------------------
    using ConstantsLookupMap = std::unordered_map<std::string, Value>;
    inline ConstantsLookupMap RegisteredConstants;

    inline void RegisterConstants(const std::string& name, Value value) {
        RegisteredConstants[name] = value;
    }
    inline Value* getConstants( const std::string& name) {
        auto it = RegisteredConstants.find(name);
        if (it != RegisteredConstants.end())
            return &it->second;
        else
            return nullptr;
    }
     // -------------------------------------------------------------

} //namespace


