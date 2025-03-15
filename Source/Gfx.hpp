#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>

#include "Korelib.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "imgui.h"

class Gfx final : public korelib::StaticOnlyClass
{
    friend class Input;

public:
    static constexpr auto DEFAULT_VERTEX_SHADER = R"(
        #version 460 core

        layout (location = 0) in vec3 inPos;
        layout (location = 1) in vec3 inColor;
        layout (location = 2) in vec2 inUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 vertexColor;
        out vec2 uv;

        void main()
        {
            gl_Position = projection * view * model * vec4(inPos, 1.0);
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

    struct Transform
    {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
    };
    

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicies);
        Mesh(std::vector<Vertex>&& vertices, const std::vector<uint32_t>&& indicies);

        const std::vector<Vertex>& vertices() const;
        const std::vector<uint32_t>& indicies() const;
        VertexBufferObjectType vertexBufferObject() const;
        VertexArrayObjectType vertexArrayObject() const;

        Transform& transform();
        glm::mat4& model();

    private:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indicies;
        VertexBufferObjectType m_vertexBufferObject;
        VertexArrayObjectType m_vertexArrayObject;

        glm::mat4 m_model;
        Transform m_transform;
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


    class Camera
    {
        static constexpr glm::vec3 VECTOR_UP = { 0.0f, 1.0f, 0.0f };
        static constexpr glm::vec3 VECTOR_FRONT = { 0.0f, 0.0f, -1.0f };

    public:
        Camera(float fov, float near, float far);
        void unwrap(uint32_t width, uint32_t height);

        void update();

        float& fov();
        float& near();
        float& far();

        float& pitch();
        float& yaw();
        float& roll();

        glm::vec3& position();

        const glm::vec3& up() const;
        const glm::vec3& front() const;

        const glm::mat4& view() const;
        const glm::mat4& projection() const;

    private:
        float m_fov;
        float m_near;
        float m_far;

        float m_pitch {};
        float m_yaw { -90.0f };
        float m_roll {};

        glm::vec3 m_position {};
        glm::vec3 m_front {};
        glm::vec3 m_right {};
        glm::vec3 m_up {};

        glm::mat4 m_view;
        glm::mat4 m_projection;
    };

public:
    static void initialize(uint32_t width, uint32_t height, const std::string& title, WindowFlags flags);
    static void beginFrame();
    static float deltaTime();
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
    static void setShaderMat4x4Value(ShaderType shaderProgram, const std::string& name, const glm::mat4& value);
    static void setShaderProgram(ShaderType program);
    static void destroyShader(ShaderType shader);
    static void drawIndexedGeometry(const glm::mat4& model, const std::vector<Vertex>& vertecies, const std::vector<uint32_t>& indicies, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets);
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
    static inline float g_deltaTime {};
    static inline float g_lastFrameTime{};
};


class Input : public korelib::StaticOnlyClass
{
public:
    static bool GetKeyDown(uint32_t keyCode);
    static glm::vec2 GetMousePosition();
};
