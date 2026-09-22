//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Array - a simple one dimensional Array
//-----------------------------------------------------------------------------
#pragma once

#include <vector>
#include "Environment.h"
#include "Value.h"
#include "ValueObject.h"
#include "FunctionMap.h"

namespace DreiZehn {

    struct Array_Object : public ValueObject {
        std::vector<Value> elements;
        Array_Object() : ValueObject(ValueObjectType::Userdata) {}
        ~Array_Object() = default;
    };
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    void RegisterArrayFunctions(Environment& env) {
        using namespace FunctionMap;

        RegisterFunction("array.new", [&env](std::vector<Value>& args, Value& ret) -> bool {
            Array_Object* arr = new Array_Object();
            ret = Value(arr);
            env.addToGarbageCollection(arr);
            return true;
        });

        // ---------------------------------------------------------------------
        RegisterFunction("array.push", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 2) {
                Tools::errorf("usage: array.push arr value\n");
                return false;
            }
            auto* arr = dynamic_cast<Array_Object*>(args[0].asPointerObject());
            if (!arr) return false;

            arr->elements.push_back(args[1]);
            ret = args[1];
            return true;
        });

        RegisterFunction("array.pop", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 1) {
                Tools::errorf("usage: array.pop arr\n");
                return false;
            }
            auto* arr = dynamic_cast<Array_Object*>(args[0].asPointerObject());
            if (!arr) return false;

            if (arr->elements.size() > 0) {
                ret = Value(arr->elements.back());
                arr->elements.pop_back();
            } else {
                ret= Value(nullptr);
            }
            return true;
        });
        // ---------------------------------------------------------------------
        RegisterFunction("array.size", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 1) return false;
            auto* arr = dynamic_cast<Array_Object*>(args[0].asPointerObject());
            if (!arr) return false;

            ret = Value(static_cast<double>(arr->elements.size()));
            return true;
        });

        // ---------------------------------------------------------------------
        RegisterFunction("array.get", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 2) return false;
            auto* arr = dynamic_cast<Array_Object*>(args[0].asPointerObject());
            int idx = static_cast<int>(args[1].getDouble());

            if (!arr || idx < 0 || idx >= static_cast<int>(arr->elements.size())) {
                Tools::errorf("array.get: Index out of bounds oder ungültiges Array\n");
                return false;
            }

            ret = arr->elements[idx];
            return true;
        });

        // ---------------------------------------------------------------------
        RegisterFunction("array.set", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 3) return false;
            auto* arr = dynamic_cast<Array_Object*>(args[0].asPointerObject());
            int idx = static_cast<int>(args[1].getDouble());

            if (!arr || idx < 0 || idx >= static_cast<int>(arr->elements.size())) {
                Tools::errorf("array.set: Index out of bounds\n");
                return false;
            }

            arr->elements[idx] = args[2];
            ret = args[2];
            return true;
        });
        // ---------------------------------------------------------------------
    }

}
