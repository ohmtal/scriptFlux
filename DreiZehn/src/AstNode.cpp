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
    Value CallExpression::evaluate(Environment& env) {
        if (FunctionMap::IsCFunction(funcName)) {

            std::vector<Value> evaluatedArgs;
            for (auto& argExpr : arguments) {
                evaluatedArgs.push_back(argExpr->evaluate(env));
            }

            Value returnValue;
            bool success = FunctionMap::RegisteredFunctions[funcName](evaluatedArgs, returnValue);

            if (!success) {
                Tools::errorf("Error in function: %s\n", funcName.c_str());
            }
            return returnValue;
        }
        else
        if (FunctionMap::IsScriptFunction(funcName)) {
            auto& func = FunctionMap::RegisteredScriptFunctions[funcName];
            Environment localEnv(&env);

            for (size_t i = 0; i < func.parameterNames.size(); ++i) {
                if (i < arguments.size()) {
                    Value evaluatedArg = arguments[i]->evaluate(env);
                    localEnv.setVariable(func.parameterNames[i], evaluatedArg);
                }
            }
            Value functionResult;
            for (auto& statement : func.body) {
                FlowSignal sig = env.execute(statement.get(), localEnv);

                if (sig == FlowSignal::Return) {
                    Value retVal = localEnv.getVariable("__return_value__");
                    return retVal;
                }
            }

            return functionResult;
        }
        else {
            Tools::errorf("Unknown command: %s\n", funcName.c_str());
            return Value();
        }

    }
    // -------------------------------------------------------------------------

    Value BinaryExpression::evaluate(Environment& env)  {
        if (!left.get() || !right.get()) {
            Tools::errorf("Parse Error!");
            return Value();
        }
        Value lVal = left->evaluate(env);
        Value rVal = right->evaluate(env);


        if ((!lVal.isDouble() && !lVal.isInt()) || (!rVal.isDouble() && !rVal.isInt()) ) {
            Tools::errorf("TYPE ERROR: We need numbers here");
            return Value();
        }

        if (op == TokenType::Greater) {
            double l = lVal.isInt() ? lVal.asInt() : lVal.asDouble();
            double r = rVal.isInt() ? rVal.asInt() : rVal.asDouble();
            return Value(l > r ? 1 : 0);
        }
        if (op == TokenType::Less) {
            double l = lVal.isInt() ? lVal.asInt() : lVal.asDouble();
            double r = rVal.isInt() ? rVal.asInt() : rVal.asDouble();
            return Value(l < r ? 1 : 0);
        }
        if (op == TokenType::Equal) {
            double l = lVal.isInt() ? lVal.asInt() : lVal.asDouble();
            double r = rVal.isInt() ? rVal.asInt() : rVal.asDouble();
            return Value(l == r ? 1 : 0);
        }

        if (lVal.isInt() && rVal.isInt()) {
            if (op == TokenType::Plus) return Value(lVal.asInt() + rVal.asInt());
            if (op == TokenType::Minus) return Value(lVal.asInt() - rVal.asInt());
            if (op == TokenType::Mul) return Value(lVal.asInt() * rVal.asInt());
            if (op == TokenType::Div) return Value(lVal.asInt() / rVal.asInt());
        }
        double lNum = lVal.isInt() ? lVal.asInt() : lVal.asDouble();
        double rNum = rVal.isInt() ? rVal.asInt() : rVal.asDouble();

        if (op == TokenType::Plus) return Value(lNum + rNum);
        if (op == TokenType::Minus) return Value(lNum - rNum);
        if (op == TokenType::Mul) return Value(lNum * rNum);
        if (op == TokenType::Div) return Value(lNum / rNum);

        return Value();
    }


    // -------------------------------------------------------------------------
}
