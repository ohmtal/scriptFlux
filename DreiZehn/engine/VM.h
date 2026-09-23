//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// The power machine :)
//  1. Build AST
//  2. chunk.rawInstructions
//  3. threadChunk(chunk)
//  4. runDirectThreadedVM(chunk)
//-----------------------------------------------------------------------------
#pragma once
#include <vector>
#include <iostream>
#include "VMStructure.h"

namespace DreiZehn {

//-----------------------------------------------------------------------------

    namespace VMRuntime {
        inline const void* dispatch_table[256] = { nullptr };
        inline bool is_initialized = false;
    }
//-----------------------------------------------------------------------------

inline Value runDirectThreadedVM(BytecodeChunk& chunk, /*HACK int numLocals*/std::vector<Value>& locals) {

    if (!VMRuntime::is_initialized) {
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Constant)] = &&do_constant;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::GetLocal)] = &&do_get_local;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::SetLocal)] = &&do_set_local;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Add)]      = &&do_add;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Sub)]      = &&do_sub;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Mul)]      = &&do_mul;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Div)]      = &&do_div;
        VMRuntime::dispatch_table[static_cast<int>(OpCode::Exit)]     = &&do_exit;
        VMRuntime::is_initialized = true;
    }
    if (chunk.rawInstructions.empty() && chunk.executableInstructions.empty()) {
        return Value(0.0);
    }
    if (chunk.executableInstructions.empty()) {
        chunk.executableInstructions.reserve(chunk.rawInstructions.size());
        for (const auto& raw : chunk.rawInstructions) {
            void* addr = const_cast<void*>(VMRuntime::dispatch_table[static_cast<int>(raw.op)]);
            chunk.executableInstructions.push_back({addr, raw.arg});
        }
    }

    std::vector<Value> vmStack;

    // FIXME HACK porting variables >>>>
    // disabled: std::vector<Value> locals(std::max(numLocals, 64), Value(0.0));
    vmStack.reserve(32);
    // <<<<


    const Op* ip = chunk.executableInstructions.data();

    #define DISPATCH() goto *(ip->labelAddress)

    // lets got
    DISPATCH();

    // -------------------------------
    do_constant: {
        vmStack.push_back(chunk.constants[ip->argument]);
        ip++; DISPATCH();

    }
    // -------------------------------
    do_get_local: {
        vmStack.push_back(locals[ip->argument]);
        ip++; DISPATCH();
    }
    // -------------------------------
    do_set_local: {
        locals[ip->argument] = vmStack.back();
        ip++;
        DISPATCH();
    }
    // -------------------------------
    do_add: {
        Value b = vmStack.back(); vmStack.pop_back();
        Value a = vmStack.back(); vmStack.pop_back();
        // Nutzt deine NaN-Boxing Logik (Beispiel: als Double)
        vmStack.push_back(Value(a.getDouble() + b.getDouble()));
        ip++;
        DISPATCH();
    }
    // -------------------------------
    do_sub: {
        Value b = vmStack.back(); vmStack.pop_back();
        Value a = vmStack.back(); vmStack.pop_back();
        vmStack.push_back(Value(a.getDouble() - b.getDouble()));
        ip++;
        DISPATCH();
    }
    // -------------------------------
    do_mul: {
        Value b = vmStack.back(); vmStack.pop_back();
        Value a = vmStack.back(); vmStack.pop_back();
        vmStack.push_back(Value(a.getDouble() * b.getDouble()));
        ip++;
        DISPATCH();
    }
    // -------------------------------
    do_div: {
        Value b = vmStack.back(); vmStack.pop_back();
        Value a = vmStack.back(); vmStack.pop_back();
        vmStack.push_back(Value(a.getDouble() / b.getDouble()));
        ip++;
        DISPATCH();
    }
    // -------------------------------
    do_exit: {
        return vmStack.empty() ? Value(0.0) : vmStack.back();
    }

    #undef DISPATCH
}

//-----------------------------------------------------------------------------
// --------------
inline void initBytecodeEngine() {
    // // BytecodeChunk emptyChunk;
    // // runDirectThreadedVM(emptyChunk, 0);
}
// --------------
// -----------------------------------------------------------------------------
} // namespace DreiZehn
