//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Core Commands
//-----------------------------------------------------------------------------
#pragma once

#include "FunctionMap.h"
#include "ScriptLoader.h"

namespace DreiZehn {


    void RegisterCoreFunctions( Environment& env) {
        using namespace FunctionMap;
        // -------- print --------------
        RegisterFunction("print", [](std::vector<Value>& args, Value& ret) -> bool {
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
        // -------- basic random --------------
        RegisterFunction("random", [](std::vector<Value>& args, Value& ret) -> bool {
            // Generiere eine Zufallszahl (z.B. zwischen 0 und 100)
            ret = Value(static_cast<int>(rand() % 100));
            return true;
        });

        // -------- add function "+" --------------
        RegisterFunction("add", [](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() < 2) return false;

            double sum = 0;
            for (auto& v : args) {
                if (v.isInt()) sum += v.asInt();
                if (v.isDouble()) sum += v.asDouble();
            }

            ret = Value(sum);
            return true;
        });
        // ------------------------------------------
        DreiZehn::FunctionMap::RegisterFunction("run", [&env](std::vector<Value>& args, Value& ret) -> bool {
            if (args.size() < 1) {
                Tools::errorf("file name requires for run\n");
                return false;
            }

            if (args[0].isPointer()) {
                auto* obj = static_cast<ValueObject*>(args[0].asPointer());
                if (obj->type == ValueObjectType::String) {
                    auto* strObj = static_cast<StringValueObject*>(obj);

                    Tools::printf("Loading Script: %s\n", strObj->value.c_str());
                    bool success = RunScriptFile(strObj->value, env);

                    ret = Value(success ? 1 : 0);
                    return success;
                }
            }

            Tools::errorf("file name requires for run\n");
            return false;
        });
        // ------------------------------------------
        DreiZehn::FunctionMap::RegisterFunction("concat", [&env](std::vector<Value>& args, Value& ret) -> bool {
            std::string resultStr = "";

            for (const auto& val : args) {
                if (val.isInt()) {
                    resultStr += std::to_string(val.asInt());
                }
                else if (val.isDouble()) {
                    std::string dStr = std::to_string(val.asDouble());
                    dStr.erase(dStr.find_last_not_of('0') + 1, std::string::npos);
                    if (dStr.back() == '.') dStr.pop_back();
                    resultStr += dStr;
                }
                else if (val.isPointer()) {
                    auto* obj = static_cast<ValueObject*>(val.asPointer());
                    if (obj && obj->type == ValueObjectType::String) {
                        auto* strObj = static_cast<StringValueObject*>(obj);
                        resultStr += strObj->value;
                    }
                }
            }

            auto* newStrObj = new StringValueObject(resultStr);
            env.addToGarbageCollection(newStrObj);
            ret = Value(newStrObj);
            return true;
        });

        // ------------------------------------------

    } //RegisterCoreFunctions

} //namespace
