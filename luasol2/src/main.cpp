//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Lua / Sol2 Playground
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include <sol/sol.hpp>

#include "BaseFlux/Main.h"
#include "../../include/ImConsole.h"

BaseFlux::Main app;
ImConsole console;
sol::state lua;

//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    console.AddLog("%s", message);
}
//-----------------------------------------------------------------------------
// Test  function ....
void Hello(std::string caption) {
    SDL_Log("Hello %s", caption.c_str());
}

// Test  struct / class
struct foo {
    float myFloat = 5.f;
    bool bar(float value) {
        SDL_Log("BAR! value=%f", value);
        return value > myFloat;
    }
};

//-----------------------------------------------------------------------------
void initConsole() {
    SDL_SetLogOutputFunction(ConsoleLogFunction, nullptr);

    console.OnCommand = [&](ImConsole* console, const char* cmd) {
        std::string cmdStr = cmd;
        if (cmdStr == "foo") {
            try {
                lua.safe_script(R"(
                    local f = foo.new()
                    print("myFloat is: " .. f.myFloat)

                    f.myFloat = 10.0
                    local result = f:bar(15.5)

                    print("result: " .. tostring(result))
                )");
            } catch (const sol::error& e) {
                console->AddLog("Lua Error: %s", e.what());
            } catch (const std::exception& e) {
                console->AddLog("C++ Error: %s", e.what());
            }

            return;
        }

        // Lua commands:
        try {

            auto result = lua.safe_script(cmd, sol::script_pass_on_error);

            if (!result.valid()) {
                sol::error err = result;
                console->AddLog("Error: %s", err.what());
                return;
            }

            if (result.return_count() == 0 || result[0].is<sol::nil_t>()) {
                return;
            }

            sol::object obj = result[0];
            std::string output = lua["tostring"](obj);

            console->AddLog("%s", output.c_str());

        } catch (const std::exception &e) {
            console->AddLog("C++ Exception: %s", e.what());
        }

    };

}
//-----------------------------------------------------------------------------


void initLua() {
    // lua.open_libraries(sol::lib::base);
    lua.open_libraries(
        sol::lib::base
        ,sol::lib::package
        ,sol::lib::coroutine
        ,sol::lib::os
        ,sol::lib::math
        ,sol::lib::table
        ,sol::lib::string
        ,sol::lib::debug
        ,sol::lib::io

    );


    // ------ register function ----
    lua.set_function("HelloLua", &Hello);

    // ----- register struct -----
    lua.new_usertype<foo>("foo",
        // constructor
        sol::constructors<foo()>(),
        // Member var
        "myFloat", &foo::myFloat,
        // Member function
        "bar", &foo::bar
    );


    lua.set_function("print", [](sol::variadic_args args) {
        std::string output;
        for (auto it = args.begin(); it != args.end(); ++it) {
            std::string s = (*it).as<std::string>();
            output += s + (std::next(it) == args.end() ? "" : "\t");
        }
        SDL_Log("%s", output.c_str());
    });

}
//-----------------------------------------------------------------------------
void onDraw(SDL_Renderer* renderer) {
    console.Draw("Lua Console",nullptr);
}
//-----------------------------------------------------------------------------
void shutDown() {
    SDL_SetLogOutputFunction(nullptr, nullptr);
}
//-----------------------------------------------------------------------------
bool initApp() {

    if ( !app.InitSDL() ) return false;
    app.initImGui();

    app.OnRender = [&](SDL_Renderer* renderer) {
        onDraw(renderer);
    };
    return true;
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    app.getSettings() = {
        .Company = "ScriptFlux",
        .Caption = "LuaFlux",
        .Version = "0.260420.14",
    };

    if (!initApp()) return 1;

    initLua();
    initConsole();
    SDL_Log("[info] Welcome to Lua Console!");
    app.Execute();
    shutDown();

    return 0;
}
