//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// ChaiFlux Playground
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include "BaseFlux/Main.h"

int main(int argc, char* argv[]) {
    BaseFlux::Main app;

    app.getSettings() = {
        .Caption = "ChaiFlux"
        .ScreenSize = { 300, 300},
        .sdlWindowFlagsOverwrite = SDL_WINDOW_BORDERLESS
    };
    if ( !app.InitSDL() ) return 1;
    app.initImGui();


    app.OnRender = [&](SDL_Renderer* renderer) {
        // ~~~~ FullscreenWindow ~~~~
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        if (ImGui::Begin("Test", nullptr, window_flags))
        {
            ImGui::SeparatorText("Hello World!");

        }
        ImGui::End();
    };

    app.Execute();

    return 0;
}
