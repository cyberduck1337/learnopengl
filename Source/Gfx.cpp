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

Gfx::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies) : m_vertices(vertices), m_indicies(indicies), m_vertexBufferObject(createVertexBufferObject()), m_vertexArrayObject(createVertexArrayObject())
{
}

Gfx::Mesh::Mesh(std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indicies) : m_vertices(std::move(vertices)), m_indicies(std::move(indicies)), m_vertexBufferObject(createVertexBufferObject()), m_vertexArrayObject(createVertexArrayObject())
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

    ShaderType defaultVertexShader = compileShader(DEFAULT_VERTEX_SHADER, ShaderKind::VERTEX);
    ShaderType defaultFragmentShader = compileShader(DEFAULT_FRAGMENT_SHADER, ShaderKind::FRAGMENT);

    g_defaultShader = linkShaderProgram(defaultVertexShader, defaultFragmentShader);
    
    destroyShader(defaultVertexShader);
    destroyShader(defaultFragmentShader);
}

void Gfx::beginFrame()
{
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

void Gfx::destroyShader(Gfx::ShaderType shader)
{
    glDeleteShader(shader);
}

void Gfx::drawIndexedGeometry(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets)
{
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
    glUseProgram(Gfx::defaultShaderProgram());
    glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, indicies.data());
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
