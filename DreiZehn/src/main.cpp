#include <vector>
#include <iostream>

#include "DreiZehn.h"


int main() {

    using namespace DreiZehn;

    Environment env;
    RegisterCoreFunctions(env);

    std::string line;

    Tools::printf("DreiZehn CLI ready.\n");
    while (true) {
        Tools::printf ("> ");
        std::getline(std::cin, line);
        if (line == "exit") break;

        // 1. Lexer
        Lexer lexer(line);
        auto tokens = lexer.tokenize();

        // 2. Parser
        Parser parser(tokens);
        auto ast = parser.parseLine();

        // 3. Evaluator
        env.execute(ast.get());
    }
    env.shutDown();
    return 0;
}



// ===================================================================================
// #include "Value.h"
// #include "ValueObject.h"
// void testValue(const std::vector<Value>& args) {
//     for (const auto& val : args) {
//         if (val.isInt()) {
//             std::cout << "Int : " << val.asInt() << "\n";
//         } else if (val.isDouble()) {
//             std::cout << "Double : " << val.asDouble() << "\n";
//         // } else if (arg.isPointer()) {
//         //     std::cout << "Pointer : " << arg.asPointer() << "\n";
//         // }
//         } else if (val.isPointer()) {
//             ValueObject* obj = static_cast<ValueObject*>(val.asPointer());
//
//             if (obj->type == ValueObjectType::String) {
//                 auto* strObj = static_cast<StringValueObject*>(obj);
//                 std::cout << "String: " << strObj->value << "\n";
//             }
//             else
//                 std::cout << "Pointer : " << val.asPointer() << "\n";
//         }
//     }
// }
//
// int main() {
//     printf("DreiZehn Ready,\n");
//
//
//     // ----
//     Value v1 = 42;          // Integer
//
//     Value v2 = 3.1415;      // Double
//
//     StringValueObject strObj = StringValueObject("Hello DreiZehn");
//     Value v3 = &strObj;
//
//     int foo = 4711;
//     UserdataValueObject fooObj = UserdataValueObject(&foo);
//     Value v4 = &fooObj;
//
//     std::vector<Value> args = {v1, v2, v3, v4};
//     testValue(args);
//
//     // ----
//
//     // ============ MAIN LOOP ============
//     std::string code;
//     while (true) {
//         std::cout << "> ";
//         std::getline(std::cin, code);
//         if (code == "exit") break;  // Exit command
//
//     }
//
//     return 0;
// }
