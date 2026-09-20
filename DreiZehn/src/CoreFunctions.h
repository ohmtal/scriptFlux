//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Core Commands
//-----------------------------------------------------------------------------
#pragma once

#include "Environment.h"


namespace DreiZehn {

    void RegisterCoreFunctions( Environment& env) {

        // -------- print --------------
        env.registerFunction("print", [](std::vector<Value>& args, Value& ret) -> bool {
            for (const auto& val : args) {
                if (val.isInt()) {
                    Tools::printf("%d ", val.asInt());
                } else if (val.isDouble()) {
                    Tools::printf("%f ", val.asDouble());
                } else if (val.isPointer()) {
                    auto* obj = static_cast<ValueObject*>(val.asPointer());
                    if (obj->type == ValueObjectType::String) {
                        auto* strObj = static_cast<StringValueObject*>(obj);
                        Tools::printf("%s ", strObj->value.c_str());
                    }
                }
            }
            Tools::printf("\n");
            return true;
        });
        // -------- **** --------------
    } //RegisterCoreFunctions

} //namespace
