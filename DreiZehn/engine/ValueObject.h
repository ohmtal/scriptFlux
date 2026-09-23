//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>
#include "Tools.h"
#include "SymbolTable.h"

namespace DreiZehn{
class Value;

enum class ValueObjectType { String, Userdata };

// TODO ... i need a id system for that ?! or as static .. later
struct ValueObjectMethod {
    uint32_t mSymbolId = 0;
    bool mIsMethod = true;
    std::string mName;
    std::string mHelp;
    uint32_t mMinParams = 0;
    uint32_t mMaxParams = 0;

    // for static define
    ValueObjectMethod(){};
    // method
    ValueObjectMethod(std::string name,  uint32_t minParams, uint32_t maxParams, std::string help)
    : mIsMethod(true),mName(name),  mHelp(help), mMinParams(minParams), mMaxParams(maxParams) {
        mSymbolId = SymbolTable::insert(name);
    }
    // property
    ValueObjectMethod(std::string name,  std::string help)
    : mIsMethod(false),mName(name),  mHelp(help), mMinParams(0), mMaxParams(0) {
        mSymbolId = SymbolTable::insert(name);
    }

    bool ValidateArgs( std::vector<Value>& args);

};

struct ValueObject {
    ValueObjectType mType;
    virtual ~ValueObject() = default;

    inline virtual bool onMethodCall(uint32_t methodNameSymbolId,  std::vector<Value>& args, Value& ret) {
        Tools::errorf("Runtime Error: Method or field %s not found.\n",SymbolTable::getName(methodNameSymbolId).c_str());
        return false;
    }


protected:
    ValueObject(ValueObjectType t) : mType(t) {}
};

struct StringValueObject : public ValueObject {
    std::string mValue;
    StringValueObject(std::string str) : ValueObject(ValueObjectType::String), mValue(std::move(str)) {}
    bool onMethodCall(uint32_t methodNameSymbolId,  std::vector<Value>& args, Value& ret) override;
};

struct UserdataValueObject : public ValueObject {
    void* mRawCustomPointer;
    UserdataValueObject(void* p) : ValueObject(ValueObjectType::Userdata), mRawCustomPointer(p) {}
};


} //namespace
