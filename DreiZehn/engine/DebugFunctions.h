//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Core Commands
//-----------------------------------------------------------------------------
#pragma once
#include "FunctionMap.h"
#include "Globals.h"

namespace DreiZehn {


    void RegisterDebugFunctions( ) {
         using namespace FunctionMap;
        // ---------------------------------------------------------------------
        RegisterFunction("debug.toggle", [](std::vector<Value>& args, Value& ret) -> bool {
            Globals::gDumpStateNodes = ! Globals::gDumpStateNodes;
            return true;
        });
        // ---------------------------------------------------------------------
        RegisterFunction("debug.printenv", [](std::vector<Value>& args, Value& ret) -> bool {
            Tools::printf("Current env: %p\n", (void*)Globals::gCurEnv);
            return true;
        });
        // ---------------------------------------------------------------------
        RegisterFunction("debug.printfn", [](std::vector<Value>& args, Value& ret) -> bool {

            const bool printScriptFunc = args.size() > 0 && args.at(0).getInt() == 1;
            Tools::printf("  --- Functions [%zu] --- \n", RegisteredFunctions.size());
            for (const auto& [key, value] : RegisteredFunctions) {
                Tools::printf("  - %s \n", key.c_str());
            }

            if (printScriptFunc) {
                Tools::printf("  --- Script Functions [%zu] --- \n", RegisteredScriptFunctions.size());
                for (const auto& [key, value] : RegisteredScriptFunctions) {
                    Tools::printf("  - %s \n", key.c_str());
                }
            }


            return true;
        });

    } //RegisterDebugFunctions

} //namespace
