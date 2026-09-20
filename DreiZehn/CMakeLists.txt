cmake_minimum_required(VERSION 3.24)
# -----------------------------------------------------------------------------
project(DreiZehn C CXX)
set(PROJECT_SRC
    src/main.cpp
    src/Value.h
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
