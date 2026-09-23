//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Compiler Scope for Variable Rgisters woring with SymbolTable
// Example:
//   uint32_t symId = SymbolTable::insert("localX");
//   int slot = currentScope.getOrAssignSlot(symId);
//-----------------------------------------------------------------------------
#pragma once

#include <unordered_map>
#include <cstdint>
#include <string>

namespace DreiZehn {

class CompilerScope {
private:
    std::unordered_map<uint32_t, int> mSymbolIdToSlot;
    std::vector<uint32_t> mSlotToSymbolId;
    int mNextSlotIndex = 0;

public:
    int getOrAssignSlot(uint32_t symbolId) {
        auto it = mSymbolIdToSlot.find(symbolId);
        if (it != mSymbolIdToSlot.end()) {
            return it->second;
        }

        int assignedSlot = mNextSlotIndex++;
        mSymbolIdToSlot[symbolId] = assignedSlot;
        mSlotToSymbolId.push_back(symbolId);
        return assignedSlot;
    }

    uint32_t getSymbolIdForSlot(int slot) const {
        return mSlotToSymbolId.at(slot);
    }

    int getLocalCount() const { return mNextSlotIndex; }
};

} // namespace DreiZehn
