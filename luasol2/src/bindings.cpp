#include "bindings.h"
#include "BaseFlux/Main.h"
#include <SDL3/SDL.h>

namespace BaseFlux {

    void bindMain(sol::state& lua) {
        // lua.new_usertype<SDL_Window>("SDL_Window");

        auto type = lua.new_usertype<BaseFlux::Main>("BaseFlux::Main");
        type.set(sol::call_constructor, sol::constructors<BaseFlux::Main()>());
        // type["getWindow"] = &BaseFlux::Main::getWindow;
        // type["getWindow"] = [](BaseFlux::Main& self) {return sol::lightuserdata(self.getWindow());};
        type["getWindow"] = [](BaseFlux::Main& self) -> void* {
            return static_cast<void*>(self.getWindow());
        };



    }
}; //namespace
