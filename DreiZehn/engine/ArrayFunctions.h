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
        std::vector<Value> mElements;
        ArrayValueObject() : ValueObject(ValueObjectType::Userdata) {}
        ~ArrayValueObject() = default;

        // -------------------------------------------------------------------------
        inline bool onMethodCall(uint32_t methodId,  std::vector<Value>& args, Value& ret) override {

            if (methodId == SymbolTable::insert( "push")) {
                if (args.size() != 1 ) return false;
                mElements.push_back(args[0]);
                ret = Value(args[0]);
                return true;
            }
            else
            if (methodId == SymbolTable::insert( "pop")) {
                if (args.size() != 0 ) return false;
                if (mElements.size() > 0) {
                    ret = Value(mElements.back());
                    mElements.pop_back();
                } else {
                    ret= Value();
                }
                return true;
            }
            else
            if (methodId == SymbolTable::insert( "size")) {
                if (args.size() != 0 ) return false;
                ret = Value(static_cast<int>(mElements.size()));
                return true;
            }
            else
            if (methodId == SymbolTable::insert( "get") || methodId == SymbolTable::insert( "at")) {
                if (args.size() != 1) return false;
                if (mElements.size() > args[0].getInt()) {
                    ret = Value(mElements.at(args[0].getInt()));
                }
                return true;
            }
            else
            if (methodId == SymbolTable::insert( "set" ))  {
                if (args.size() != 2 || mElements.size() <= args[0].getInt()) return false;
                mElements[args[0].getInt()] = args[1];
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

        // ---------------------------------------------------------------------
        RegisterFunction("Array.push", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 2) {
                Tools::errorf("usage: Array.push arr value\n");
                return false;
            }
            auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
            if (!arr) return false;

            arr->mElements.push_back(args[1]);
            ret = args[1];
            return true;
        });

        RegisterFunction("Array.pop", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 1) {
                Tools::errorf("usage: Array.pop arr\n");
                return false;
            }
            auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
            if (!arr) return false;

            if (arr->mElements.size() > 0) {
                ret = Value(arr->mElements.back());
                arr->mElements.pop_back();
            } else {
                ret= Value();
            }
            return true;
        });
        // ---------------------------------------------------------------------
        RegisterFunction("Array.size", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 1) return false;
            auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
            if (!arr) return false;

            ret = Value(static_cast<double>(arr->mElements.size()));
            return true;
        });

        // ---------------------------------------------------------------------
        RegisterFunction("Array.get", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 2) return false;
            auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
            int idx = static_cast<int>(args[1].getDouble());

            if (!arr || idx < 0 || idx >= static_cast<int>(arr->mElements.size())) {
                Tools::errorf("Array.get: Index out of bounds oder ungültiges Array\n");
                return false;
            }

            ret = arr->mElements[idx];
            return true;
        });

        // ---------------------------------------------------------------------
        RegisterFunction("Array.set", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() != 3) return false;
            auto* arr = dynamic_cast<ArrayValueObject*>(args[0].asPointerObject());
            int idx = static_cast<int>(args[1].getDouble());

            if (!arr || idx < 0 || idx >= static_cast<int>(arr->mElements.size())) {
                Tools::errorf("Array.set: Index out of bounds\n");
                return false;
            }

            arr->mElements[idx] = args[2];
            ret = args[2];
            return true;
        });
        // ---------------------------------------------------------------------
    }

}
