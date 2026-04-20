//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// ChaiFlux Playground
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
#include <chaiscript/chaiscript.hpp>

#include "BaseFlux/Main.h"
#include "ImConsole.h"

BaseFlux::Main app;
ImConsole console;
std::unique_ptr<chaiscript::ChaiScript> chai;
//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    console.AddLog("%s", message);
}
//-----------------------------------------------------------------------------
// Test Chai function ....
void HelloChai(std::string caption) {
    SDL_Log("Hello %s", caption.c_str());
}

// Test Chai struct / class
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
                chai->eval(R"(
                    var f = foo();
                    print("myFloat is: ${f.myFloat}");
                    f.myFloat = 10.0;
                    var result = f.bar(15.5);
                    print("result: ${result}");
                )");
            } catch (const std::exception &e) {
                console->AddLog("Error: %s", e.what());
            }
            return;
        }
       if (cmdStr == "reset") {
          chai->get_locals().clear();
          return;
       }

        // redirect command to chai
        try {
            auto result = chai->eval(cmd);
            if (result.get_type_info().bare_equal(chaiscript::user_type<void>())) {
                // console->AddLog("[info] called void..");
                return;
            }
            auto to_string_func = chai->eval<std::function<std::string(chaiscript::Boxed_Value)>>("to_string");
            std::string output = to_string_func(result);
            console->AddLog("%s", output.c_str());

        } catch (const std::exception &e) {
            console->AddLog("Error: %s", e.what());
        }
    };

}
//-----------------------------------------------------------------------------
void initChai() {

    chai = std::make_unique<chaiscript::ChaiScript>();

    // ------ register function ----
    chai->add(chaiscript::fun(&HelloChai), "HelloChai");

    // ----- register struct -----
    chai->add(chaiscript::user_type<foo>(), "foo");
    chai->add(chaiscript::constructor<foo()>(), "foo");
    chai->add(chaiscript::fun(&foo::myFloat), "myFloat");
    chai->add(chaiscript::fun(&foo::bar), "bar");

    // ----- redirect print and puts -----
    chai->add(chaiscript::fun([](const std::string &msg) {
        SDL_Log("%s", msg.c_str());
    }), "print");

    chai->add(chaiscript::fun([](const std::string &msg) {
        SDL_Log("%s", msg.c_str());
    }), "puts");

}
//-----------------------------------------------------------------------------
void onDraw(SDL_Renderer* renderer) {
    console.Draw("ChaiScript Console",nullptr);
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
        .Company = "ChaiFlux",
        .Caption = "ChaiFlux",
        .Version = "0.260420.5",
    };

    if (!initApp()) return 1;

    initChai();
    initConsole();
    SDL_Log("[info] Welcome to ChaiScript Console!");
    app.Execute();
    shutDown();

    return 0;
}
