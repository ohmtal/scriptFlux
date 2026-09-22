//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>
#include "Tools.h"

namespace DreiZehn{
class Value;

enum class ValueObjectType { String, Userdata };

struct ValueObject {
    ValueObjectType mType;
    virtual ~ValueObject() = default;
    inline virtual bool onMethodCall(std::string methodName,  std::vector<Value>& args, Value& ret) {
        Tools::errorf("Runtime Error: Method or field %s not found.\n",methodName.c_str());
        return false;
    }
protected:
    ValueObject(ValueObjectType t) : mType(t) {}
};

struct StringValueObject : public ValueObject {
    std::string mValue;
    StringValueObject(std::string str) : ValueObject(ValueObjectType::String), mValue(std::move(str)) {}
    bool onMethodCall(std::string methodName,  std::vector<Value>& args, Value& ret) override;
};

struct UserdataValueObject : public ValueObject {
    void* mRawCustomPointer;
    UserdataValueObject(void* p) : ValueObject(ValueObjectType::Userdata), mRawCustomPointer(p) {}
};


} //namespace
