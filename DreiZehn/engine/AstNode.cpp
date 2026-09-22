//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Value Evaluator
//-----------------------------------------------------------------------------
#include "Environment.h"
#include "FunctionMap.h"

namespace DreiZehn {
    // -------------------------------------------------------------------------
    Value LiteralExpression::evaluate(Environment& env) {
        if (type == TokenType::Number) {
            if (rawValue.find('.') != std::string::npos) {
                return Value(std::stod(rawValue));
            }
            return Value(std::stoi(rawValue));
        }

        if (type == TokenType::StringLiteral) {
            auto* strObj = new StringValueObject(rawValue);

            env.addToGarbageCollection(strObj);

            return Value(strObj);
        }
        return Value(); // Fallback
    }
    // -------------------------------------------------------------------------
    Value VariableExpression::evaluate(Environment& env) {
        return env.getVariable(name);

    }
    // -------------------------------------------------------------------------
    Value MethodExpression::evaluate(Environment& env) {
        Value objectPointer = env.getVariable(pointerName);
        if (!objectPointer.isPointer()) {
            Tools::errorf("RunTime Error: Object %s not found.\n", pointerName.c_str());
            return Value();
        }
        ValueObject* obj = dynamic_cast<ValueObject*>(objectPointer.asPointerObject());
        if (!obj) {
            Tools::errorf("RunTime Error: Invalid Object: %s.\n", pointerName.c_str());
            return Value();
        }
        std::vector<Value> evaluatedArgs;
        for (auto& argExpr : arguments) {
            if (argExpr) evaluatedArgs.push_back(argExpr->evaluate(env));
        }

        Value returnValue = Value(0);

        if (!obj->onMethodCall(methodName, evaluatedArgs, returnValue)) {
            Tools::errorf("Runtime Error in method call: %s -> %s\n", pointerName.c_str(), methodName.c_str());
        }
        return returnValue;

    }
    // -------------------------------------------------------------------------
    Value CallExpression::evaluate(Environment& env) {
        FunctionMap::CallBack* cb = nullptr;
        cb = FunctionMap::GetCFunction(funcName);
        if (cb) {

            std::vector<Value> evaluatedArgs;
            for (auto& argExpr : arguments) {
                if (argExpr) evaluatedArgs.push_back(argExpr->evaluate(env));
            }

            Value returnValue = Value(0);
            // bool success = FunctionMap::RegisteredFunctions[funcName](evaluatedArgs, returnValue);
            bool success = (*cb)(evaluatedArgs, returnValue);

            if (!success) {
                Tools::errorf("Runtime Error in function: %s\n", funcName.c_str());
            }
            return returnValue;
        }

        const FunctionMap::ScriptFunction* sf = FunctionMap::GetScriptFunction(funcName);

        if (sf) {
            // auto& func = FunctionMap::RegisteredScriptFunctions[funcName];
            auto& func = *(sf);
            Environment localEnv(&env);

            for (size_t i = 0; i < func.parameterNames.size(); ++i) {
                if (i < arguments.size()) {
                    Value evaluatedArg = arguments[i]->evaluate(env);
                    localEnv.setVariable(func.parameterNames[i], evaluatedArg);
                }
            }
            Value functionResult = Value(0);
            for (auto& statement : func.body) {
                FlowSignal sig = env.execute(statement.get(), localEnv);

                if (sig == FlowSignal::Return) {
                    Value retVal = localEnv.getVariable("__return_value__");
                    return retVal;
                }
            }

            return functionResult;
        }

        Tools::errorf("Unknown command: %s\n", funcName.c_str());
        return Value();

    }
    // -------------------------------------------------------------------------

    Value BinaryExpression::evaluate(Environment& env)  {
        if (!left.get() || !right.get()) {
            Tools::PrintParseError("left or right is missing:");
            return Value();
        }
        Value lVal = left->evaluate(env);
        Value rVal = right->evaluate(env);

        if (op == TokenType::Greater) {
            double l = lVal.getDouble();
            double r = rVal.getDouble();
            return Value((l - r) > EPSILON ? 1 : 0);
        }
        else
        if (op == TokenType::GreaterEqual) {
            double l = lVal.getDouble();
            double r = rVal.getDouble();
            return Value(l > (r - EPSILON) ? 1 : 0);
        }
        else
        if (op == TokenType::Less) {
            double l = lVal.getDouble();
            double r = rVal.getDouble();
            return Value((r - l) > EPSILON ? 1 : 0);
        }
        else
            if (op == TokenType::LowerEqual) {
                double l = lVal.getDouble();
                double r = rVal.getDouble();
                return Value(l < (r + EPSILON) ? 1 : 0);
            }
        else
        if (op == TokenType::Equal) {
            if (lVal.isPointer() && rVal.isPointer()) {
                return Value(lVal.asPointer() == rVal.asPointer() ? 1 : 0);
            }
            double l = lVal.getDouble();
            double r = rVal.getDouble();
            return Value(std::abs(l - r) < EPSILON ? 1 : 0);
        }
        else
        if (op == TokenType::NotEqual) {
            if (lVal.isPointer() && rVal.isPointer()) {
                return Value(lVal.asPointer() != rVal.asPointer() ? 1 : 0);
            }
            double l = lVal.getDouble();
            double r = rVal.getDouble();
            return Value(std::abs(l - r) < EPSILON ? 0 : 1);
        }
        else
        if (op == TokenType::Or) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l || r);
        }
        else
        if (op == TokenType::And) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l && r);
        }
        else
        if (op == TokenType::BitAnd) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l & r);
        }
        else
        if (op == TokenType::BitOr) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l | r);
        }
        else
        if (op == TokenType::SHL) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l << r);
        }
        else
        if (op == TokenType::SHR) {
            int l = lVal.getInt();
            int r = rVal.getInt();
            return Value( l >> r);
        }

        if (lVal.isInt() && rVal.isInt()) {
            if (op == TokenType::Plus) return Value(lVal.asInt() + rVal.asInt());
            if (op == TokenType::Minus) return Value(lVal.asInt() - rVal.asInt());
            if (op == TokenType::Mul) return Value(lVal.asInt() * rVal.asInt());
            if (op == TokenType::Div) return Value(lVal.asInt() / rVal.asInt());
        }
        double lNum = lVal.getDouble();
        double rNum = rVal.getDouble();

        if (op == TokenType::Plus) return Value(lNum + rNum);
        if (op == TokenType::Minus) return Value(lNum - rNum);
        if (op == TokenType::Mul) return Value(lNum * rNum);
        if (op == TokenType::Div) return Value(lNum / rNum);

        return Value();
    }


    // -------------------------------------------------------------------------

}
