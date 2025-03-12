#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#include "Korelib.hpp"
#include "glm/glm.hpp"

class Gfx final : public korelib::StaticOnlyClass
{
public:
    static constexpr auto DEFAULT_VERTEX_SHADER = R"(
        #version 460 core

        layout (location = 0) in vec3 inPos;
        layout (location = 1) in vec3 inColor;
        layout (location = 2) in vec2 inUV;

        out vec3 vertexColor;
        out vec2 uv;

        void main()
        {
            gl_Position = vec4(inPos, 1.0);
            vertexColor = inColor;
            uv = inUV;
        }
    )";

    static constexpr auto DEFAULT_FRAGMENT_SHADER = R"(
        #version 460 core

        out vec4 FragColor;

        in vec3 vertexColor;
        in vec2 uv;

        uniform sampler2D textureData;

        void main()
        {
            FragColor = texture(textureData, uv) * vec4(vertexColor, 1.0);
        }
    )";
    
public:
    enum class WindowFlags : uint32_t
    {
        NONE = 0x00000000
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 uv;
    };

    using WindowType = struct GLFWwindow*;
    using WindowReizeDelegate = korelib::Delegate<void, uint32_t, uint32_t>;

    using VertexBufferObjectType = uint32_t; // stores data
    using VertexArrayObjectType = uint32_t; // stores pointers in data buffer
    using ShaderType = uint32_t;
    using TextureIdType = uint32_t;

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies);
        Mesh(std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indicies);

        const std::vector<Vertex>& vertices() const;
        const std::vector<uint32_t>& indicies() const;
        VertexBufferObjectType vertexBufferObject() const;
        VertexArrayObjectType vertexArrayObject() const;

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indicies;
        VertexBufferObjectType m_vertexBufferObject;
        VertexArrayObjectType m_vertexArrayObject;
    };

    enum class ShaderKind : uint8_t
    {
        VERTEX,
        FRAGMENT
    };

    struct Attribute
    {
        enum class Type : uint32_t
        {
            BYTE,
            UNSIGNED_BYTE,
            SHORT,
            UNSIGNED_SHORT,
            INTEGER,
            UNSIGNED_INTEGER,
            FLOAT
        };

        uint32_t index;
        uint32_t numComponents;
        size_t stride;
        Type type;
        uintptr_t offset;
        bool aligned;
    };

    class Texture
    {
    public:
        Texture(uint8_t* data, int32_t width, int32_t height);
        ~Texture();

        static Texture fromFile(const std::filesystem::path& path);

        TextureIdType textureId() const;
        const uint8_t* const data() const;

        int32_t width() const;
        int32_t height() const;

    private:
        TextureIdType m_textureId;
        uint8_t* m_data;
        int32_t m_width;
        int32_t m_height;
    };

public:
    static void initialize(uint32_t width, uint32_t height, const std::string& title, WindowFlags flags);
    static void beginFrame();
    static bool windowShouldClose();
    static void setClearColor(float r, float g, float b, float a);
    static void clearBackground();
    static void swap();
    static VertexBufferObjectType createVertexBufferObject();
    static VertexArrayObjectType createVertexArrayObject();
    static ShaderType compileShader(const std::string& source, ShaderKind kind);
    static ShaderType linkShaderProgram(ShaderType vertexShader, ShaderType fragmentShader);
    static void setShaderUniformBoolValue(ShaderType shaderProgram, const std::string& name, bool value);
    static void setShaderUniformIntValue(ShaderType shaderProgram, const std::string& name, int32_t value);
    static void setShaderUniformIntValue(ShaderType shaderProgram, const std::string& name, float value);
    static void setShaderProgram(ShaderType program);
    static void destroyShader(ShaderType shader);
    static void drawIndexedGeometry(const std::vector<Vertex>& vertecies, const std::vector<uint32_t>& indicies, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets);
    static TextureIdType createTextureObject();
    static void bindTexture(const Texture& texture);
    static void updateTextureData(const Texture& texture);
    static void endFrame();
    static void destroy();

    static WindowReizeDelegate& onWindowSizeChangedDelegate()
    {
        return g_onWindowSizeChanged;
    }

    static ShaderType defaultShaderProgram()
    {
        return g_defaultShader;
    }

private:
    static inline WindowType g_window { nullptr };
    static inline WindowReizeDelegate g_onWindowSizeChanged {};
    static inline ShaderType g_defaultShader {};
};
