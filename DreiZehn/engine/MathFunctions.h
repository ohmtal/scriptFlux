//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Core Commands
//-----------------------------------------------------------------------------
#pragma once
#include <ctime>
#include "FunctionMap.h"
#include "ScriptLoader.h"

namespace DreiZehn {


    void RegisterMathFunctions( ) {
        std::srand(std::time(nullptr)); //setup random seed

        using namespace FunctionMap;

        // -------- basic random --------------
        // FIXME ?!
        // RegisterFunction("math.random", [](std::vector<Value>& args, Value& ret) -> bool {
        //     ret = Value(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
        //     return true;
        // });
        RegisterFunction("math.randomf", [](std::vector<Value>& args, Value& ret) -> bool {
            ret = Value(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
            return true;
        });


    } //RegisterMathFunctions

} //namespace
