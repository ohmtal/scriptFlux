//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Value Object
//-----------------------------------------------------------------------------
#include <vector>

// #include <stdlib.h>
#include <string.h>
// #include <ctype.h>
// #include <cstdarg>

#include "Value.h"
#include "ValueObject.h"


namespace DreiZehn {
    // -------------------------------------------------------------------------
    bool StringValueObject::onMethodCall(std::string methodName,  std::vector<Value>& args, Value& ret) {

        if (methodName == "toNumber") {
            char* endptr = nullptr;
            double resDouble = std::strtod(value.c_str(), &endptr);
            if (value.empty() || *endptr != '\0') {
                ret = Value(0);
            } else {
                ret = Value(resDouble);
            }
            return true;
        }
        return false;
    }

    // -------------------------------------------------------------------------

}
