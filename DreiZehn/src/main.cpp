#include <vector>
#include <iostream>
#include <sstream>

#include "engine/DreiZehn.h"
#include "engine/DebugFunctions.h"
#include "engine/ArrayFunctions.h"
#include "engine/SDL3Functions.h"

void RegisterUserFunc() {
    using namespace DreiZehn;
    FunctionMap::RegisterFunction("fnFoo", [](std::vector<Value>& args, Value& ret) -> bool {
        printf("The Foo was here ...\n");
        ret = Value(4711.0815);
        return true;
    });
}


// FIXME preprocssor!

int main(int argc, char* argv[]) {
    using namespace DreiZehn;

    initBytecodeEngine();
    Environment env;
    RegisterCoreFunctions(env);
    RegisterArrayFunctions(env);
    RegisterMathFunctions();
    RegisterDebugFunctions();
    RegisterUserFunc();

    RegisterSDL3Functions(env);


    if (argc > 1) {
        std::string scriptPath = argv[1];

        bool success = RunScriptFile(scriptPath, env);

        env.shutDown();

        return success ? 0 : 1;
    }

    // -------------------------------------------------------------------------
    // Console Mode - handling multi line input for "for" and "fn"
    // -------------------------------------------------------------------------
    std::string line;

    std::vector<OpenBlock> blockStack;

    while (true) {
        for (size_t i = 0; i < blockStack.size(); ++i) std::cout << ".. ";
        std::cout << (blockStack.empty() ? "> " : "");

        std::getline(std::cin, line);
        if (line == "exit") break;

        std::stringstream stream(line);
        RunScriptStream(stream, env);

    }
    env.shutDown();
    return 0;
}
