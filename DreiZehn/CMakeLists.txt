cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, defaulting to Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


set(SANITIZE_DEFAULT OFF)
set(BUILD_WITH_SANITIZE ${SANITIZE_DEFAULT} CACHE BOOL "Build Application with -fsanitize=address -g")

# -----------------------------------------------------------------------------
project(DreiZehn CXX)

set(PROJECT_SRC
    engine/Value.h
    engine/ValueObject.h
    engine/Lexer.h
    engine/Parser.h
    engine/AstNode.h
    engine/AstNode.cpp
    engine/Environment.h
    engine/Tools.h
    engine/CoreFunctions.h
    engine/MathFunctions.h
    engine/DebugFunctions.h

    engine/ScriptLoader.h
    engine/ScriptLoader.cpp

    engine/Globals.h
    engine/DreiZehn.h

    # --- testing SDL3 bindings
    engine/SDL3Functions.h


    src/main.cpp
)

# ------------- SDL3
find_package(SDL3 REQUIRED)

# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)



target_include_directories(${PROJECT_NAME} PRIVATE
     "${CMAKE_CURRENT_SOURCE_DIR}/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
)

if (BUILD_WITH_SANITIZE)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -g)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()
