#pragma once

#include <cstdint>
#include <cstring>
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
#include "glm/gtx/euler_angles.hpp"

#include "imgui.h"
#include "ImGuizmo.h"
#include "GraphEditor.h"
#include "ImCurveEdit.h"
#include "ImGradient.h"
#include "ImSequencer.h"

class Gfx final : public korelib::StaticOnlyClass
{
    friend class Input;

public:
    static constexpr auto DEFAULT_VERTEX_SHADER = R"(
        #version 460 core

        layout (location = 0) in vec3 inPos;
        layout (location = 1) in vec2 inUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec2 uv;

        void main()
        {
            gl_Position = projection * view * model * vec4(inPos, 1.0);
            uv = inUV;
        }
    )";

    static constexpr auto DEFAULT_FRAGMENT_SHADER = R"(
        #version 460 core

        out vec4 FragColor;

        in vec2 uv;

        uniform sampler2D u_texture;

        void main()
        {
            FragColor = texture(u_texture, -uv);
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
        static constexpr glm::vec3 VECTOR_UP = { 0.0f, 1.0f, 0.0f };
        static constexpr glm::vec3 VECTOR_FRONT = { 0.0f, 0.0f, -1.0f };

        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        glm::vec3 eulerAngles() const;
        glm::vec3 direction() const;
        glm::vec3 front() const;
        glm::vec3 right() const;
        glm::vec3 up() const;

        void rotate(const glm::vec3& eulerAngles);
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
        Texture(uint8_t* data, int32_t width, int32_t height, int32_t channels);
        Texture(const Texture& other);
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
        int32_t m_channels;
    };

public:
    static void initialize(uint32_t width, uint32_t height, const std::string& title, WindowFlags flags);
    static void beginFrame();
    static float deltaTime();
    static bool windowShouldClose();
    static glm::uvec2 getWindowSize();
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
    static void drawIndexedGeometry(const Transform& transform, const std::vector<Vertex>& vertecies, const std::vector<std::array<uint32_t, 3>>& indicies, ShaderType shaderProgram, VertexBufferObjectType vertexBufferObject, VertexArrayObjectType vertexArrayObject, const std::vector<Attribute>& attributesDataOffsets);
    static TextureIdType createTextureObject();
    static void bindTexture(const Texture& texture);
    static void updateTextureData(const Texture& texture);
    static std::shared_ptr<class Camera> getActiveCamera();
    static void setActiveCamera(std::shared_ptr<Camera> camera);
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
    static inline std::shared_ptr<class Camera> g_activeCamera{};
};


class Input : public korelib::StaticOnlyClass
{
public:
    static bool GetKeyDown(uint32_t keyCode);
    static glm::vec2 GetMousePosition();
    static bool GetMouseButtonDown(uint32_t keyCode);
};
