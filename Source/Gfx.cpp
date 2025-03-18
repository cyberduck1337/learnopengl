#include "Gfx.hpp"
#include "Assertion.hpp"
#include "fmt/format.h"
#include "RuntimeException.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

Gfx::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies) : m_vertices(vertices), m_indicies(indicies), m_vertexBufferObject(createVertexBufferObject()), m_vertexArrayObject(createVertexArrayObject()), m_model(1.0f)
{
}

Gfx::Mesh::Mesh(std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indicies) : m_vertices(std::move(vertices)), m_indicies(std::move(indicies)), m_vertexBufferObject(createVertexBufferObject()), m_vertexArrayObject(createVertexArrayObject()), m_model(1.0f)
{
}

const std::vector<Gfx::Vertex>& Gfx::Mesh::vertices() const
{
    return m_vertices;
}

const std::vector<uint32_t>& Gfx::Mesh::indicies() const
{
    return m_indicies;
}

Gfx::VertexBufferObjectType Gfx::Mesh::vertexBufferObject() const
{
    return m_vertexBufferObject;
}

Gfx::VertexArrayObjectType Gfx::Mesh::vertexArrayObject() const
{
    return m_vertexArrayObject;
}

Gfx::Transform& Gfx::Mesh::transform()
{
    [[maybe_unused]] glm::vec3 skew;
    [[maybe_unused]] glm::vec4 perspective;

    glm::decompose(m_model, m_transform.scale, m_transform.rotation, m_transform.position, skew, perspective);
    return m_transform;
}

glm::mat4& Gfx::Mesh::model()
{
    return m_model;
}

Gfx::Texture::Texture(uint8_t* data, int32_t width, int32_t height) : m_textureId(Gfx::createTextureObject()), m_data(data), m_width(width), m_height(height)
{
}

Gfx::Texture::~Texture()
{
    stbi_image_free(m_data);
}

Gfx::Texture Gfx::Texture::fromFile(const std::filesystem::path& path)
{
    KORELIB_VERIFY_THROW(std::filesystem::exists(path), korelib::RuntimeException, fmt::format("File '{}' does not exist!", path.string()));

    int32_t width {};
    int32_t height {};
    int32_t channels {};

    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
    KORELIB_VERIFY_THROW(data != nullptr, korelib::RuntimeException, fmt::format("Failed to load image: {}", path.string()));

    return { data, width,  height};
}

Gfx::TextureIdType Gfx::Texture::textureId() const
{
    return m_textureId;
}

const uint8_t* const Gfx::Texture::data() const
{
    return m_data;
}

int32_t Gfx::Texture::width() const
{
    return m_width;
}

int32_t Gfx::Texture::height() const
{
    return m_height;
}

Gfx::Camera::Camera(float fov, float near, float far) : m_fov(fov), m_near(near), m_far(far)
{
}

void Gfx::Camera::unwrap(uint32_t width, uint32_t height)
{
    m_projection = glm::perspective(glm::radians(m_fov), static_cast<float>(width) / static_cast<float>(height), m_near, m_far);
}

void Gfx::Camera::update()
{
    glm::vec3 direction {
        glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)),
        glm::sin(glm::radians(m_pitch)),
        glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))
    };

    m_front = glm::normalize(std::move(direction));

    m_right = glm::normalize(glm::cross(m_front, VECTOR_UP));
    m_up = glm::normalize(glm::cross(m_right, m_front));
    m_view = glm::lookAt(m_position, m_position + m_front, m_up);
}

float& Gfx::Camera::fov()
{
    return m_fov;
}

float& Gfx::Camera::near()
{
    return m_near;
}

float& Gfx::Camera::far()
{
    return m_far;
}

float& Gfx::Camera::pitch()
{
    return m_pitch;
}

float& Gfx::Camera::yaw()
{
    return m_yaw;
}

float& Gfx::Camera::roll()
{
    return m_roll;
}

glm::vec3& Gfx::Camera::position()
{
    return m_position;
}

const glm::vec3& Gfx::Camera::up() const
{
    return m_up;
}

const glm::vec3& Gfx::Camera::front() const
{
    return m_front;
}

const glm::mat4& Gfx::Camera::view() const
{
    return m_view;
}

const glm::mat4& Gfx::Camera::projection() const
{
    return m_projection;
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

void Gfx::drawIndexedGeometry(const glm::mat4& model, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets)
{
    Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "model", model);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // 0. select VertexBufferObject to work with
    glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<Vertex>::value_type) * vertices.size(), vertices.data(), GL_STATIC_DRAW); // 1. copy vertex data to the video card
    glBindVertexArray(vertexArrayObject); // 2. select VertexArrayObject to work with
    for (auto&& attributePointer : attributesDataOffsets) // 3. set the vertex attributes pointers
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
    glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, indicies.data());
}

Gfx::TextureIdType Gfx::createTextureObject()
{
    Gfx::TextureIdType texture{};
    glGenTextures(1, &texture);

    return texture;
}

void Gfx::bindTexture(const Texture& texture)
{
    glBindTexture(GL_TEXTURE_2D, texture.textureId());
}

void Gfx::updateTextureData(const Texture& texture)
{
    bindTexture(texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width(), texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data());
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
