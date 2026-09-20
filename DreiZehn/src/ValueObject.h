//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>

namespace DreiZehn{

//TODO flexible system ...
enum class ValueObjectType { String, Userdata };

struct ValueObject {
    ValueObjectType type;
    virtual ~ValueObject() = default;
protected:
    ValueObject(ValueObjectType t) : type(t) {}
};

struct StringValueObject : public ValueObject {
    std::string value;
    StringValueObject(std::string str) : ValueObject(ValueObjectType::String), value(std::move(str)) {}
};

struct UserdataValueObject : public ValueObject {
    void* rawCustomPointer;
    UserdataValueObject(void* p) : ValueObject(ValueObjectType::Userdata), rawCustomPointer(p) {}
};
} //namespace
