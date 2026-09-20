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

# -----------------------------------------------------------------------------
project(DreiZehn CXX)

set(PROJECT_SRC
    src/Value.h
    src/ValueObject.h
    src/Lexer.h
    src/Parser.h
    src/AstNode.h
    src/Environment.h
    src/Tools.h
    src/CoreFunctions.h

    src/DreiZehn.h

    src/main.cpp
)

# ------------- Main
add_executable(${PROJECT_NAME}
    ${PROJECT_SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)



target_include_directories(${PROJECT_NAME} PRIVATE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
)
