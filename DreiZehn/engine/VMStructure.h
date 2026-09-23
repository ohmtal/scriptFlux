//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// VM Structure
//-----------------------------------------------------------------------------
#pragma once
#pragma once
#include <vector>
#include <cstdint>
namespace DreiZehn {

enum class OpCode : uint16_t {
    Constant,   // push const to stack
    GetLocal,   // fetch var
    SetLocal,   // write var

    Add, Sub, Mul, Div, // math

    Exit        // bye bye
};

// raw command
struct RawInstruction {
    OpCode op;
    uintptr_t arg = 0;
};

// final command for direct threading
struct Op {
    void* labelAddress; // &&myCommand
    uintptr_t argument; // backpack data
};

// the code chunk
struct BytecodeChunk {
    std::vector<RawInstruction> rawInstructions;
    std::vector<Op> executableInstructions;
    std::vector<Value> constants;

    int addConstant(Value val) {
        constants.push_back(val);
        return static_cast<int>(constants.size() - 1);
    }

    void emit(OpCode op, uintptr_t arg = 0) {
        rawInstructions.push_back({op, arg});
    }
};

} // namespace DreiZehn
