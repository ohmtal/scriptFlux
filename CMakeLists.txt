cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
# Project settings - this is all you have to modify:
project(ChaiFlux)
set(PROJECT_SRC
    src/main.cpp
)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------- BaseFlux
include(FetchContent)
FetchContent_Declare(
  BaseFlux
  # --- directly from github:
  GIT_REPOSITORY https://github.com/ohmtal/BaseFlux.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(BaseFlux)

# ------------- ChaiScript
FetchContent_Declare(
  chaiscript
  GIT_REPOSITORY https://github.com/ChaiScript/ChaiScript.git
  GIT_TAG        v6.1.0
)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CHAISCRIPT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(chaiscript)


# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_precompile_headers(ChaiFlux PRIVATE
    <chaiscript/chaiscript.hpp>
    <chaiscript/chaiscript_stdlib.hpp>
)

target_include_directories(ChaiFlux PRIVATE ${chaiscript_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    BaseFlux
)
