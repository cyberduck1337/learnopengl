cmake_minimum_required(VERSION 3.11)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git GIT_TAG v0.1.36)
FetchContent_Declare(glm GIT_REPOSITORY	https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(korelib GIT_REPOSITORY https://github.com/cyberduck1337/korelib.git GIT_TAG master)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/blitz-research/stb.git GIT_TAG master)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.4-docking)
FetchContent_Declare(ctti GIT_REPOSITORY https://github.com/Manu343726/ctti.git GIT_TAG master)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glad)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(korelib)
FetchContent_MakeAvailable(stb)
FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(ctti)

add_executable(learnopengl
    Source/main.cpp
    Source/Gfx.hpp
    Source/Gfx.cpp
    Source/SceneGraph.hpp
    Source/SceneGraph.cpp

    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(learnopengl PUBLIC
    korelib
    glfw
    glad
    glm
    ctti
)

target_include_directories(learnopengl PUBLIC 
    ${stb_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
)

target_compile_definitions(learnopengl PUBLIC
    GLM_ENABLE_EXPERIMENTAL
)

add_custom_command(TARGET learnopengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Resources
)
