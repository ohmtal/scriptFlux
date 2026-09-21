//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

namespace DreiZehn {
    class Environment;
    bool RunScriptFile(const std::string& filename, Environment& env);
}
