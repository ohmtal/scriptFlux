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
            double resDouble = std::strtod(mValue.c_str(), &endptr);
            if (mValue.empty() || *endptr != '\0') {
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
