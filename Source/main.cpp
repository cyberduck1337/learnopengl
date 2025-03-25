
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

int main(int argc, char** argv)
{
    static constexpr uint32_t INITIAL_WINDOW_WIDTH = 800;
    static constexpr uint32_t INITIAL_WINDOW_HEIGHT = 600;

    Gfx::initialize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Learn OpenGL", Gfx::WindowFlags::NONE);

    Gfx::Texture texture = Gfx::Texture::fromFile("Resources/Textures/Grass_Block.jpg");

    std::shared_ptr<Scene> scene = Scene::create("MyScene");
    std::shared_ptr<GameObject> cameraGameObject = scene->addGameObject("MainCamera", {0.0f, 0.0f, -2.5f});
    std::shared_ptr<Camera> cameraComponent = cameraGameObject->addComponent<Camera>(45, 0.1f, 100);
    std::shared_ptr<FlyCameraController> flyCameraController = cameraGameObject->addComponent<FlyCameraController>();

    static Gfx::Mesh cube{
        // vertex array [position, uv]
        {
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
        },
        // indicies array
        {
             2,  1,  0,   0,  3,  2,  // front
             7,  5,  6,   7,  4,  5,  // back
             8, 11, 10,   9,  8, 10,  // left
            15, 12, 13,  13, 14, 15,  // right
            17, 16, 19,  18, 17, 19,  // top
            21, 22, 20,  23, 20, 22,  // bottom
        }
    };

    static const std::vector<Gfx::Attribute> attributes
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

    Gfx::Transform& meshTransform = cube.transform();
    meshTransform.scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = glm::degrees(glm::eulerAngles(meshTransform.rotation));

    Gfx::setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!Gfx::windowShouldClose())
    {
        Gfx::beginFrame();
        scene->update();

        rotation.y += 100 * Gfx::deltaTime();
        if (rotation.y >= 360.0f)
        {
            rotation.y = 0;
        }
        meshTransform.rotation = glm::quat(glm::radians(rotation));

        Gfx::setShaderProgram(Gfx::defaultShaderProgram());

        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "view", cameraComponent->view());
        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "projection", cameraComponent->projection());

        Gfx::updateTextureData(texture);
        Gfx::drawIndexedGeometry(cube.transform(), cube.vertices(), cube.indicies(), Gfx::defaultShaderProgram(), cube.vertexBufferObject(), cube.vertexArrayObject(), attributes);

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

        ImGui::Begin("Object");
        ImGui::InputFloat("Position.x", &meshTransform.position.x);
        ImGui::InputFloat("Position.y", &meshTransform.position.y);
        ImGui::InputFloat("Position.z", &meshTransform.position.z);
        if (ImGui::SliderFloat("Rotation.x", &rotation.x, 0, 359.9f) || ImGui::SliderFloat("Rotation.y", &rotation.y, 0, 359.9f) || ImGui::SliderFloat("Rotation.z", &rotation.z, 0, 359.9f))
        {
            meshTransform.rotation = glm::quat(glm::radians(rotation));
        }
        ImGui::InputFloat("Scale.x", &meshTransform.scale.x);
        ImGui::InputFloat("Scale.y", &meshTransform.scale.y);
        ImGui::InputFloat("Scale.z", &meshTransform.scale.z);
        ImGui::End();

        Gfx::endFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Gfx::destroy();
    return 0;
}
