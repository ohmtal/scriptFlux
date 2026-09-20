//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// NaN Boxing Value
//-----------------------------------------------------------------------------
#pragma once

#include <iostream>
#include <cstdint>
#include <bit>
#include <cassert>

#include "ValueObject.h"

namespace DreiZehn{

constexpr uint64_t QNAN_MASK = 0x7FF8000000000000ULL;
constexpr uint64_t TAG_INT   = 0x0001000000000000ULL; // Integer
constexpr uint64_t TAG_PTR   = 0x0002000000000000ULL; // Pointer

class Value {
private:
    uint64_t bits; // 8 byte value

    explicit Value(uint64_t b) : bits(b) {}

public:
    // Default constructor
    Value() : bits(0) {}

    Value(double d) {
        bits = std::bit_cast<uint64_t>(d);
    }

    Value(int32_t i) {
        // NaN Mask + Int-Tag + 32-Bit Integers
        bits = QNAN_MASK | TAG_INT | static_cast<uint32_t>(i);
    }

    Value(ValueObject* obj) {
        uint64_t ptrBits = std::bit_cast<uint64_t>(obj);
        bits = QNAN_MASK | TAG_PTR | (ptrBits & 0x0000FFFFFFFFFFFFULL);
    }

    // --- Typ-Check ---
    bool isDouble()  const { return (bits & QNAN_MASK) != QNAN_MASK; }
    bool isInt()     const { return (bits & (QNAN_MASK | TAG_INT)) == (QNAN_MASK | TAG_INT); }
    bool isPointer() const { return (bits & (QNAN_MASK | TAG_PTR)) == (QNAN_MASK | TAG_PTR); }

    // --- Getter (Unboxing) ---
    double asDouble() const {
        assert(isDouble());
        return std::bit_cast<double>(bits);
    }

    int32_t asInt() const {
        assert(isInt());
        return static_cast<int32_t>(bits & 0xFFFFFFFFULL);
    }

    void* asPointer() const {
        assert(isPointer());
        uint64_t ptrBits = bits & 0x0000FFFFFFFFFFFFULL;
        return std::bit_cast<void*>(ptrBits);
    }
};
} //namespace
