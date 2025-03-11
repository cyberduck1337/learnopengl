cmake_minimum_required(VERSION 3.11)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git GIT_TAG v0.1.36)
FetchContent_Declare(glm GIT_REPOSITORY	https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(korelib GIT_REPOSITORY https://github.com/cyberduck1337/korelib.git GIT_TAG master)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/blitz-research/stb.git GIT_TAG master)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glad)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(korelib)
FetchContent_MakeAvailable(stb)

add_executable(learnopengl
    Source/main.cpp
    Source/Gfx.hpp
    Source/Gfx.cpp
)

target_link_libraries(learnopengl PUBLIC
    korelib
    glfw
    glad
    glm
)

target_include_directories(learnopengl PUBLIC 
    ${stb_SOURCE_DIR}
)
