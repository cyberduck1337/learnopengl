#include "Gfx.hpp"
#include "Assertion.hpp"
#include "fmt/format.h"
#include "RuntimeException.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

void glfwErrorCallback(int errorCode, const char* errorMessage)
{
    throw korelib::RuntimeException(fmt::format("[glfw] error: Message:\"{}\". ErrorCode:{}", errorMessage, errorCode));
}

void glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (Gfx::onWindowSizeChangedDelegate().isBound())
    {
        Gfx::onWindowSizeChangedDelegate().execute(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }
}

void Gfx::initialize(uint32_t width, uint32_t height, const std::string& title, WindowFlags flags)
{
    KORELIB_VERIFY_THROW(glfwInit() == GLFW_TRUE, korelib::RuntimeException, "Failed to initialize glfw");

    glfwSetErrorCallback(glfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    KORELIB_VERIFY_THROW(g_window != nullptr, korelib::RuntimeException, "[glfw] error: failed to initialize window");

    glfwMakeContextCurrent(g_window);

    KORELIB_VERIFY_THROW(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), korelib::RuntimeException, "Failed to initialize glad");

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(g_window, glfwWindowResizeCallback);
}

void Gfx::beginFrame()
{
    Gfx::clearBackground();
}

bool Gfx::windowShouldClose()
{
    return glfwWindowShouldClose(g_window);
}

void Gfx::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void Gfx::clearBackground()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Gfx::swap()
{
    glfwSwapBuffers(g_window);
}

void Gfx::endFrame()
{
    swap();
    glfwPollEvents();
}

void Gfx::destroy()
{
    glfwDestroyWindow(g_window);
    glfwTerminate();
}
