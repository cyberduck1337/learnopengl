#include "Gfx.hpp"
#include "Assertion.hpp"
#include "fmt/format.h"
#include "RuntimeException.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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

glm::vec3 Gfx::Transform::eulerAngles() const
{
    return glm::degrees(glm::eulerAngles(glm::normalize(rotation)));
}

glm::vec3 Gfx::Transform::direction() const
{
    const glm::vec3 rot = eulerAngles();

    return {
        glm::cos(glm::radians(rot.x)) * glm::cos(glm::radians(rot.y)),
        glm::sin(glm::radians(rot.y)),
        glm::sin(glm::radians(rot.x)) * glm::cos(glm::radians(rot.y))
    };
}

glm::vec3 Gfx::Transform::front() const
{
    return glm::normalize(direction());
}

glm::vec3 Gfx::Transform::right() const
{
    return glm::normalize(glm::cross(VECTOR_UP, front()));
}

glm::vec3 Gfx::Transform::up() const
{
    return glm::normalize(glm::cross(right(), front()));
}

void Gfx::Transform::rotate(const glm::vec3& eulerAngles)
{
    glm::quat eulerRot = glm::quat(glm::radians(eulerAngles));
    rotation *= glm::inverse(rotation) * eulerRot * rotation;
}

glm::mat4 Gfx::Transform::model() const
{
    return (glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale));
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    KORELIB_VERIFY_THROW(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), korelib::RuntimeException, "Failed to initialize glad");

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(g_window, glfwWindowResizeCallback);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    ShaderType defaultVertexShader = compileShader(DEFAULT_VERTEX_SHADER, ShaderKind::VERTEX);
    ShaderType defaultFragmentShader = compileShader(DEFAULT_FRAGMENT_SHADER, ShaderKind::FRAGMENT);

    g_defaultShader = linkShaderProgram(defaultVertexShader, defaultFragmentShader);
    
    destroyShader(defaultVertexShader);
    destroyShader(defaultFragmentShader);
}

void Gfx::beginFrame()
{
    float currentTime = glfwGetTime();
    g_deltaTime = currentTime - g_lastFrameTime;
    g_lastFrameTime = currentTime;

    clearBackground();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glm::ivec2 windowPosition = Gfx::getWindowPosition();
    glm::uvec2 windowSize = Gfx::getWindowSize();

    ImGuizmo::SetRect(windowPosition.x, windowPosition.y, windowSize.x, windowSize.y);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    glEnable(GL_DEPTH_TEST);
}

float Gfx::deltaTime()
{
    return g_deltaTime;
}

bool Gfx::windowShouldClose()
{
    return glfwWindowShouldClose(g_window);
}

glm::uvec2 Gfx::getWindowSize()
{
    int w{};
    int h{};
    glfwGetWindowSize(Gfx::g_window, &w, &h);

    return {static_cast<glm::uvec2::value_type>(w), static_cast<glm::uvec2::value_type>(h)};
}

glm::ivec2 Gfx::getWindowPosition()
{
    glm::ivec2 windowPos{};
    glfwGetWindowPos(g_window, &windowPos.x, &windowPos.y);
    return windowPos;
}

std::vector<Gfx::MonitorType> Gfx::getMonitors()
{
    int monitorsCount{};
    Gfx::MonitorType* nativeMonitors = glfwGetMonitors(&monitorsCount);

    std::vector<Gfx::MonitorType> monitors{};
    monitors.reserve(monitorsCount);
    for (int monitorIndex = 0; monitorIndex < monitorsCount; monitorIndex++)
    {
        monitors.emplace_back(nativeMonitors[monitorIndex]);
    }

    return monitors;
}

glm::ivec2 Gfx::getMonitorOffset(MonitorType monitor)
{
    glm::ivec2 monitorOffset{};
    glfwGetMonitorPos(monitor, &monitorOffset.x, &monitorOffset.y);

    return monitorOffset;
}

Gfx::VideoModeType Gfx::getVideoModeForMonitor(MonitorType monitor)
{
    return glfwGetVideoMode(monitor);
}

void Gfx::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void Gfx::clearBackground()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;
}

void Gfx::swap()
{
    glfwSwapBuffers(g_window);
}

Gfx::VertexBufferObjectType Gfx::createVertexBufferObject()
{
    Gfx::VertexBufferObjectType vertexBufferObject{};
    glGenBuffers(1, &vertexBufferObject);

    return vertexBufferObject;
}

Gfx::VertexArrayObjectType Gfx::createVertexArrayObject()
{
    Gfx::VertexArrayObjectType vertexArrayObject{};
    glGenVertexArrays(1, &vertexArrayObject);

    return vertexArrayObject;
}

Gfx::ShaderType Gfx::compileShader(const std::string& source, ShaderKind kind)
{
    Gfx::ShaderType shader {0};
    switch (kind)
    {
    case Gfx::ShaderKind::VERTEX:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case Gfx::ShaderKind::FRAGMENT:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    default:
        KORELIB_VERIFY_THROW(false, korelib::RuntimeException, fmt::format("Unexpected ShaderKind: {}", static_cast<uint8_t>(kind)));
    }

    const char* shaderSourcePtr = source.data();
    glShaderSource(shader, 1, &shaderSourcePtr, NULL);

    glCompileShader(shader);

    int  success;
    char info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, info);
        KORELIB_VERIFY_THROW(success, korelib::RuntimeException, fmt::format("Failed to compile shader: {}", info));
    }

    return shader;
}

Gfx::ShaderType Gfx::linkShaderProgram(ShaderType vertexShader, ShaderType fragmentShader)
{
    Gfx::ShaderType shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int  success;
    char info[512];
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderProgram, 512, NULL, info);
        KORELIB_VERIFY_THROW(success, korelib::RuntimeException, fmt::format("Failed to link shader: {}", info));
    }

    return shaderProgram;
}

void Gfx::setShaderUniformBoolValue(ShaderType shaderProgram, const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), static_cast<uint32_t>(value));
}

void Gfx::setShaderUniformIntValue(ShaderType shaderProgram, const std::string& name, int32_t value)
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Gfx::setShaderUniformIntValue(ShaderType shaderProgram, const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Gfx::setShaderMat4x4Value(ShaderType shaderProgram, const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Gfx::setShaderProgram(Gfx::ShaderType program)
{
    glUseProgram(program);
}

void Gfx::destroyShader(Gfx::ShaderType shader)
{
    glDeleteShader(shader);
}

void Gfx::updateVertexBufferData(VertexBufferObjectType vertexBufferObject, const std::vector<Vertex> &vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<Vertex>::value_type) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

void Gfx::drawIndexedGeometry(const Gfx::Transform& transform, const std::vector<std::array<uint32_t, 3>>& triangles, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets)
{
    Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "model", transform.model());

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindVertexArray(vertexArrayObject);
    for (auto&& attributePointer : attributesDataOffsets)
    {
        GLenum attributeType {};
        switch (attributePointer.type)
        {
        case Gfx::Attribute::Type::BYTE:
            attributeType = GL_BYTE;
            break;
        case Gfx::Attribute::Type::UNSIGNED_BYTE:
            attributeType = GL_UNSIGNED_BYTE;
            break;
        case Gfx::Attribute::Type::SHORT:
            attributeType = GL_SHORT;
            break;
        case Gfx::Attribute::Type::UNSIGNED_SHORT:
            attributeType = GL_UNSIGNED_SHORT;
            break;
        case Gfx::Attribute::Type::INTEGER:
            attributeType = GL_INT;
            break;
        case Gfx::Attribute::Type::UNSIGNED_INTEGER:
            attributeType = GL_UNSIGNED_INT;
            break;
        case Gfx::Attribute::Type::FLOAT:
            attributeType =  GL_FLOAT;
            break;
        default:
            break;
        }

        glVertexAttribPointer(attributePointer.index, attributePointer.numComponents, attributeType, attributePointer.aligned, attributePointer.stride, (void*)attributePointer.offset);
        glEnableVertexAttribArray(attributePointer.index);
    }
    glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, triangles.data());
}

Gfx::TextureIdType Gfx::createTextureObject()
{
    Gfx::TextureIdType texture{};
    glGenTextures(1, &texture);

    return texture;
}

void Gfx::setActiveTexture(TextureIdType textureId)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
}

Gfx::TextureIdType Gfx::textureFromData(uint8_t* data, int32_t width, int32_t height)
{
    Gfx::TextureIdType textureId = Gfx::createTextureObject();
    Gfx::setActiveTexture(textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    return textureId;
}

std::shared_ptr<Camera> Gfx::getActiveCamera()
{
    return g_activeCamera;
}

void Gfx::setActiveCamera(std::shared_ptr<Camera> camera)
{
    g_activeCamera = std::move(camera);
}

void Gfx::endFrame()
{
    glfwPollEvents();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    swap();
}

void Gfx::destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(g_window);
    glfwTerminate();
}

bool Input::GetKeyDown(uint32_t keyCode)
{
    return glfwGetKey(Gfx::g_window, keyCode) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
    double x; 
    double y;

    glfwGetCursorPos(Gfx::g_window, &x, &y);

    return { static_cast<float>(x), static_cast<float>(y)};
}

bool Input::GetMouseButtonDown(uint32_t keyCode)
{
    return glfwGetMouseButton(Gfx::g_window, keyCode) == GLFW_PRESS;
}
