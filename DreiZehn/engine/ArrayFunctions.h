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

    struct ArrayValueObject : public ValueObject {
        std::vector<Value> elements;
        ArrayValueObject() : ValueObject(ValueObjectType::Userdata) {}
        ~ArrayValueObject() = default;

        // -------------------------------------------------------------------------
        inline bool onMethodCall(std::string methodName,  std::vector<Value>& args, Value& ret) override {

            if (methodName == "push") {
                if (args.size() != 1 ) return false;
                elements.push_back(args[0]);
                ret = Value(args[0]);
                return true;
            }
            else
            if (methodName == "pop") {
                if (args.size() != 0 ) return false;
                if (elements.size() > 0) {
                    ret = Value(elements.back());
                    elements.pop_back();
                } else {
                    ret= Value();
                }
                return true;
            }
            else
            if (methodName == "size") {
                if (args.size() != 0 ) return false;
                ret = Value(static_cast<int>(elements.size()));
                return true;
            }
            else
            if (methodName == "get" || methodName == "at") {
                if (args.size() != 1) return false;
                if (elements.size() > args[0].getInt()) {
                    ret = Value(elements.at(args[0].getInt()));
                }
                return true;
            }
            else
            if (methodName == "set") {
                if (args.size() != 2 || elements.size() <= args[0].getInt()) return false;
                elements[args[0].getInt()] = args[1];
                ret =  args[1];
                return true;
            }

            return false;
        }
    };
    // -------------------------------------------------------------------------
    void RegisterArrayFunctions(Environment& env) {
        using namespace FunctionMap;

        RegisterFunction("Array.new", [&env](std::vector<Value>& args, Value& ret) -> bool {
            ArrayValueObject* arr = new ArrayValueObject();
            ret = Value(arr);
            env.addToGarbageCollection(arr);
            return true;
        });

        // Replaced By Methods :)
        // // // ---------------------------------------------------------------------
        // // RegisterFunction("array.push", [](std::vector<Value>& args, Value& ret) -> bool {
        // //     if (args.size() != 2) {
        // //         Tools::errorf("usage: array.push arr value\n");
        // //         return false;
        // //     }
        // //     auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
        // //     if (!arr) return false;
        // //
        // //     arr->elements.push_back(args[1]);
        // //     ret = args[1];
        // //     return true;
        // // });
        // //
        // // RegisterFunction("array.pop", [](std::vector<Value>& args, Value& ret) -> bool {
        // //     if (args.size() != 1) {
        // //         Tools::errorf("usage: array.pop arr\n");
        // //         return false;
        // //     }
        // //     auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
        // //     if (!arr) return false;
        // //
        // //     if (arr->elements.size() > 0) {
        // //         ret = Value(arr->elements.back());
        // //         arr->elements.pop_back();
        // //     } else {
        // //         ret= Value();
        // //     }
        // //     return true;
        // // });
        // // // ---------------------------------------------------------------------
        // // RegisterFunction("array.size", [](std::vector<Value>& args, Value& ret) -> bool {
        // //     if (args.size() != 1) return false;
        // //     auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
        // //     if (!arr) return false;
        // //
        // //     ret = Value(static_cast<double>(arr->elements.size()));
        // //     return true;
        // // });
        // //
        // // // ---------------------------------------------------------------------
        // // RegisterFunction("array.get", [](std::vector<Value>& args, Value& ret) -> bool {
        // //     if (args.size() != 2) return false;
        // //     auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
        // //     int idx = static_cast<int>(args[1].getDouble());
        // //
        // //     if (!arr || idx < 0 || idx >= static_cast<int>(arr->elements.size())) {
        // //         Tools::errorf("array.get: Index out of bounds oder ungültiges Array\n");
        // //         return false;
        // //     }
        // //
        // //     ret = arr->elements[idx];
        // //     return true;
        // // });
        // //
        // // // ---------------------------------------------------------------------
        // // RegisterFunction("array.set", [](std::vector<Value>& args, Value& ret) -> bool {
        // //     if (args.size() != 3) return false;
        // //     auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
        // //     int idx = static_cast<int>(args[1].getDouble());
        // //
        // //     if (!arr || idx < 0 || idx >= static_cast<int>(arr->elements.size())) {
        // //         Tools::errorf("array.set: Index out of bounds\n");
        // //         return false;
        // //     }
        // //
        // //     arr->elements[idx] = args[2];
        // //     ret = args[2];
        // //     return true;
        // // });
        // ---------------------------------------------------------------------
    }

}
