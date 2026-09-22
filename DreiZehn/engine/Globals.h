//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
#pragma once


namespace DreiZehn {
    class Environment;

    const double EPSILON = 1e-9;


namespace Globals {
    inline int currentScriptLineNumber = 0;
    inline std::string currentScriptLine =  "";

    // i want to track the current Env!
    inline Environment* gCurEnv = nullptr;
    // inline const std::string gEmptyString = ""; unused!
    inline  bool gDumpStateNodes = false; //Debug
}


} //namespace
