
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fmt/format.h"
#include "Korelib.hpp"
#include "Gfx.hpp"

#include "Components/Camera.hpp"
#include "SceneGraph.hpp"

#include <cstddef>
#include <vector>
#include <thread>
#include <limits>

class Material : public Component
{
public:
    Material(const std::shared_ptr<Entity>& parent) : Component("Material", parent), m_shaderProgram(Gfx::defaultShaderProgram())
    {
    }

    void update() override
    {
        Gfx::setShaderProgram(m_shaderProgram);

        if (m_texture.has_value())
        {
            Gfx::updateTextureData(m_texture.value());
        }

        if (const std::shared_ptr<Camera>& camera = Gfx::getActiveCamera(); camera != nullptr)
        {
            Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "view", camera->view());
            Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "projection", camera->projection());
        }
    }

    Gfx::ShaderType shaderProgram() const
    {
        return m_shaderProgram;
    }

    void setTexture(const Gfx::Texture& texture)
    {
        m_texture = texture;
    }

    std::vector<Gfx::Attribute> attributes() const
    {
        return
        {
            {
                .index = 0,
                .numComponents = 3,
                .stride = sizeof(Gfx::Vertex),
                .type = Gfx::Attribute::Type::FLOAT,
                .offset = offsetof(Gfx::Vertex, position),
                .aligned = false
            },
            {
                .index = 1,
                .numComponents = 2,
                .stride = sizeof(Gfx::Vertex),
                .type = Gfx::Attribute::Type::FLOAT,
                .offset = offsetof(Gfx::Vertex, uv),
                .aligned = false
            }
        };
    }

protected:
    Gfx::ShaderType m_shaderProgram;
    std::optional<Gfx::Texture> m_texture;
};

class MeshRenderer : public Component
{
public:
    enum class PrimitiveType : uint8_t
    {
        CUBE,
    };

public:
    MeshRenderer(const std::shared_ptr<Entity>& parent, PrimitiveType primitiveType) : Component("MeshRenderer", parent), m_vertexBufferObject(Gfx::createVertexBufferObject()), m_vertexArrayObject(Gfx::createVertexArrayObject())
    {
        m_material = gameObject()->addComponent<Material>();

        switch (primitiveType)
        {
            case PrimitiveType::CUBE:
            {
                m_vertices.reserve(24);
                m_vertices = {
                    /*[ 0]*/ {{-0.5f, -0.5f,  0.5f},     {0.0f, 1.0f / 3}},  // front  - bottom - left
                    /*[ 1]*/ {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f / 3 * 2}},  // front  - top    - left
                    /*[ 2]*/ {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f / 3 * 2}},  // front  - top    - right
                    /*[ 3]*/ {{ 0.5f, -0.5f,  0.5f},     {1.0f, 1.0f / 3}},  // front  - bottom - right
                    /*[ 4]*/ {{-0.5f, -0.5f, -0.5f},     {0.0f, 1.0f / 3}},  // back   - bottom - left
                    /*[ 5]*/ {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f / 3 * 2}},  // back   - top    - left
                    /*[ 6]*/ {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f / 3 * 2}},  // back   - top    - right
                    /*[ 7]*/ {{ 0.5f, -0.5f, -0.5f},     {1.0f, 1.0f / 3}},  // back   - bottom - right
                    /*[ 8]*/ {{-0.5f, -0.5f, -0.5f},     {0.0f, 1.0f / 3}},  // left   - bottom - back
                    /*[ 9]*/ {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f / 3 * 2}},  // left   - top    - back
                    /*[10]*/ {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f / 3 * 2}},  // left   - top    - front
                    /*[11]*/ {{-0.5f, -0.5f,  0.5f},     {1.0f, 1.0f / 3}},  // left   - bottom - front
                    /*[12]*/ {{ 0.5f, -0.5f, -0.5f},     {0.0f, 1.0f / 3}},  // right  - bottom - back
                    /*[13]*/ {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f / 3 * 2}},  // right  - top    - back
                    /*[14]*/ {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f / 3 * 2}},  // right  - top    - front
                    /*[15]*/ {{ 0.5f, -0.5f,  0.5f},     {1.0f, 1.0f / 3}},  // right  - bottom - front
                    /*[16]*/ {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f / 3 * 2}},  // top    - near   - left
                    /*[17]*/ {{-0.5f,  0.5f, -0.5f},         {0.0f, 1.0f}},  // top    - far    - left
                    /*[18]*/ {{ 0.5f,  0.5f, -0.5f},         {1.0f, 1.0f}},  // top    - far    - right
                    /*[19]*/ {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f / 3 * 2}},  // top    - near   - right
                    /*[20]*/ {{-0.5f, -0.5f,  0.5f},         {0.0f, 0.0f}},  // bottom - near   - left
                    /*[21]*/ {{-0.5f, -0.5f, -0.5f},     {0.0f, 1.0f / 3}},  // bottom - far    - left
                    /*[22]*/ {{ 0.5f, -0.5f, -0.5f},     {1.0f, 1.0f / 3}},  // bottom - far    - right
                    /*[23]*/ {{ 0.5f, -0.5f,  0.5f},         {1.0f, 0.0f}},  // bottom - near   - right
                };

                m_triangles.reserve(36);
                m_triangles = {
                     { 2,  1,  0},  {0,  3,  2}, // front
                     { 7,  5,  6},  {7,  4,  5}, // back
                     { 8, 11, 10},  {9,  8, 10}, // left
                     {15, 12, 13}, {13, 14, 15}, // right
                     {17, 16, 19}, {18, 17, 19}, // top
                     {21, 22, 20}, {23, 20, 22}, // bottom
                };
                break;
            }
            default:
                break;
        }
    }

    void update() override
    {
        Gfx::Transform& transform = gameObject()->m_transform;
        Gfx::drawIndexedGeometry(
            transform,
            m_vertices,
            m_triangles,
            m_material->shaderProgram(),
            m_vertexBufferObject,
            m_vertexArrayObject,
            m_material->attributes()
        );
    }

protected:
    std::vector<Gfx::Vertex> m_vertices;
    std::vector<std::array<uint32_t, 3>> m_triangles;

    Gfx::VertexBufferObjectType m_vertexBufferObject;
    Gfx::VertexArrayObjectType m_vertexArrayObject;

    std::shared_ptr<Material> m_material;
};

class FlyCameraController : public Component
{
public:
    FlyCameraController(const std::shared_ptr<Entity>& parent) : Component("FlyCamera", parent), speed(1.0f), sensetivity(0.1f)
    {
    }

    void update() override
    {
        Gfx::Transform& transform = std::static_pointer_cast<GameObject>(getParent())->m_transform;
        const glm::vec2 mousePosition = Input::GetMousePosition();

        if (Input::GetKeyDown(GLFW_KEY_W))
        {
            transform.position += speed * Gfx::deltaTime() * transform.front();
        }

        if (Input::GetKeyDown(GLFW_KEY_S))
        {
            transform.position -= speed * Gfx::deltaTime() * transform.front();
        }

        if (Input::GetKeyDown(GLFW_KEY_A))
        {
            transform.position -= glm::normalize(glm::cross(transform.front(), transform.up())) * speed * Gfx::deltaTime();
        }

        if (Input::GetKeyDown(GLFW_KEY_D))
        {
            transform.position += glm::normalize(glm::cross(transform.front(), transform.up())) * speed * Gfx::deltaTime();
        }

        if (Input::GetKeyDown(GLFW_KEY_SPACE))
        {
            transform.position += speed * Gfx::deltaTime() * transform.up();
        }

        if (Input::GetKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            transform.position -= speed * Gfx::deltaTime() * transform.up();
        }

        if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            float xoffset = (mousePosition.x - lastMousePosition.x) * sensetivity;
            float yoffset = (lastMousePosition.y - mousePosition.y) * sensetivity;

            glm::vec3 eluerAngles = transform.eulerAngles();
            eluerAngles.x += xoffset;
            eluerAngles.y += yoffset;
            eluerAngles.y = glm::clamp(eluerAngles.y, -89.0f, 89.0f);

            transform.rotation = glm::quat(glm::radians(eluerAngles));
        }

        lastMousePosition = mousePosition;
    }

private:
    float speed;
    float sensetivity;

    glm::vec2 lastMousePosition;
};

class CubeRotator : public Component
{
public:
    CubeRotator(const std::shared_ptr<Entity>& parent) : Component("CubeRotator", parent), rotationSpeed(50, 30, 80)
    {
    }

    void update()
    {
        glm::vec3 rot = {rotationSpeed.x * Gfx::deltaTime(), rotationSpeed.y * Gfx::deltaTime(), rotationSpeed.z * Gfx::deltaTime()}; 
        gameObject()->m_transform.rotate(std::move(rot));
    }

public:
    glm::vec3 rotationSpeed;
};

int main(int argc, char** argv)
{
    static constexpr uint32_t INITIAL_WINDOW_WIDTH = 1280;
    static constexpr uint32_t INITIAL_WINDOW_HEIGHT = 720;

    Gfx::initialize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Learn OpenGL", Gfx::WindowFlags::NONE);

    std::shared_ptr<Scene> scene = Scene::create("MyScene");
    std::shared_ptr<GameObject> cameraGameObject = scene->addGameObject("MainCamera", {-0.5f, 0.0f, -2.5f});
    cameraGameObject->m_transform.rotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)));
    std::shared_ptr<Camera> cameraComponent = cameraGameObject->addComponent<Camera>(45, 0.1f, 100);
    std::shared_ptr<FlyCameraController> flyCameraController = cameraGameObject->addComponent<FlyCameraController>();
    std::shared_ptr<GameObject> cube = scene->addGameObject("Cube", {0.0f, 0.0f, 0.0f});
    cube->addComponent<MeshRenderer>(MeshRenderer::PrimitiveType::CUBE);
    cube->addComponent<CubeRotator>();
    if (std::shared_ptr<Material> cubeMaterial = cube->getComponent<Material>(); cubeMaterial != nullptr)
    {
        Gfx::Texture texture = Gfx::Texture::fromFile("./Resources/Textures/Grass_Block.jpg");
        cubeMaterial->setTexture(std::move(texture));
    }

    Gfx::setActiveCamera(cameraComponent);
    Gfx::setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!Gfx::windowShouldClose())
    {
        Gfx::beginFrame();
        scene->update();

        if (ImGui::Begin("Camera"))
        {
            Gfx::Transform& transform = cameraGameObject->m_transform;
            {
                float position[3] { 
                    transform.position.x,
                    transform.position.y,
                    transform.position.z
                };
                if(ImGui::DragFloat3("Position", position, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()))
                {
                    transform.position = {position[0], position[1], position[2]};
                }
            }
            {
                const glm::vec3 eulerAngles = transform.eulerAngles();
                float rotation[3] { 
                    eulerAngles.x,
                    eulerAngles.y,
                    eulerAngles.z
                };
                if(ImGui::DragFloat3("Rotation", rotation, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()))
                {
                    transform.rotation = glm::quat(glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2])));
                }
            }

            ImGui::SliderFloat("near", &cameraComponent->near(), 0.0f, cameraComponent->far());
            ImGui::SliderFloat("far", &cameraComponent->far(), cameraComponent->near(), 1000);
            ImGui::SliderFloat("fov", &cameraComponent->fov(), 0, 180);

            ImGui::End();
        }

        Gfx::endFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Gfx::destroy();
    return 0;
}
