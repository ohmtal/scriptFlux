# ScriptFlux - playground for script bindings with BaseFlux


# ChaiScript
Create a Application with a ChaiScript console.

Note: It compiles SDL3, ImGui, BaseFlux and ChaiScript as precompiled headers. Linking takes some time.
Compile and run :
```
cd chaiScript
cmake -S . -B build
cmake --build build
./ChaiFlux
```
- First impressions:
    - ➕ Easy to embed.
    - ➕ C-like
    - ➕ Easy to bind
    - ➖ Linking takes ages

# Lua / Sol2
Create a Application with a Lua console.

Note: It compiles SDL3, ImGui, BaseFlux and Lua.
Compile and run :
```
cd lua
cmake -S . -B build
cmake --build build
./ChaiFlux
```
- First impressions:
    - ➕ Easy to embed.
    - ➕ Lua is very popular
    - ➕➕ sol2 make it very easy to bind
    - I like c like scripting more than lua but thats me ;)
