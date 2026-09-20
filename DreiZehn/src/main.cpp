#include <vector>
#include <iostream>
#include "Value.h"
// #include "ValueObject.h"



void testValue(const std::vector<Value>& args) {
    for (const auto& val : args) {
        if (val.isInt()) {
            std::cout << "Int : " << val.asInt() << "\n";
        } else if (val.isDouble()) {
            std::cout << "Double : " << val.asDouble() << "\n";
        // } else if (arg.isPointer()) {
        //     std::cout << "Pointer : " << arg.asPointer() << "\n";
        // }
        } else if (val.isPointer()) {
            ValueObject* obj = static_cast<ValueObject*>(val.asPointer());

            if (obj->type == ValueObjectType::String) {
                auto* strObj = static_cast<StringValueObject*>(obj);
                std::cout << "String: " << strObj->value << "\n";
            }
            else
                std::cout << "Pointer : " << val.asPointer() << "\n";

            //TODO userdata test
            // // if (obj->type == ValueObjectType::Userdata) {
            // //     auto* userObj = static_cast<UserdataValueObject*>(obj);
            // //    ....
            // // }
        }
    }
}

int main() {
    printf("DreiZehn Ready,\n");


    // ----
    Value v1 = 42;          // Integer

    Value v2 = 3.1415;      // Double

    StringValueObject valObjStr = StringValueObject("Hello DreiZehn");
    Value v3 = &valObjStr;

    std::vector<Value> args = {v1, v2, v3};
    testValue(args);

    // ----

    // ============ MAIN LOOP ============
    std::string code;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, code);
        if (code == "exit") break;  // Exit command

    }

    return 0;
}
