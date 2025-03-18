
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fmt/format.h"
#include "Korelib.hpp"
#include "Gfx.hpp"

#include <cstddef>
#include <vector>
#include <thread>
#include <limits>

int main(int argc, char** argv)
{
    static constexpr uint32_t INITIAL_WINDOW_WIDTH = 800;
    static constexpr uint32_t INITIAL_WINDOW_HEIGHT = 600;

    Gfx::initialize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Learn OpenGL", Gfx::WindowFlags::NONE);

    Gfx::Texture texture = Gfx::Texture::fromFile("Resources/Textures/Diamond_Ore.jpg");
    Gfx::Camera cam{45, 0.1f, 100};
    cam.unwrap(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    Gfx::onWindowSizeChangedDelegate().bind([&cam](uint32_t w, uint32_t h)
    {
        cam.unwrap(w, h);
    });

    static Gfx::Mesh mesh{
        // vertex array [position, color, uv]
        {
        /*[ 0]*/ {{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // front  - bottom - left
        /*[ 1]*/ {{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // front  - top    - left
        /*[ 2]*/ {{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // front  - top    - right
        /*[ 3]*/ {{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // front  - bottom - right
        /*[ 4]*/ {{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // back   - bottom - left
        /*[ 5]*/ {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // back   - top    - left
        /*[ 6]*/ {{ 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // back   - top    - right
        /*[ 7]*/ {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // back   - bottom - right
        /*[ 8]*/ {{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // left   - bottom - back
        /*[ 9]*/ {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // left   - top    - back
        /*[10]*/ {{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // left   - top    - front
        /*[11]*/ {{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // left   - bottom - front
        /*[12]*/ {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // right  - bottom - back
        /*[13]*/ {{ 0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // right  - top    - back
        /*[14]*/ {{ 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // right  - top    - front
        /*[15]*/ {{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // right  - bottom - front
        /*[16]*/ {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // top    - near   - left
        /*[17]*/ {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top    - far    - left
        /*[18]*/ {{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // top    - far    - right
        /*[19]*/ {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // top    - near   - right
        /*[20]*/ {{-0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // bottom - near   - left
        /*[21]*/ {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // bottom - far    - left
        /*[22]*/ {{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // bottom - far    - right
        /*[23]*/ {{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom - near   - right
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
            .numComponents = 3,
            .stride = sizeof(Gfx::Vertex),
            .type = Gfx::Attribute::Type::FLOAT,
            .offset = offsetof(Gfx::Vertex, color),
            .aligned = false
        },
        {
            .index = 2,
            .numComponents = 2,
            .stride = sizeof(Gfx::Vertex),
            .type = Gfx::Attribute::Type::FLOAT,
            .offset = offsetof(Gfx::Vertex, uv),
            .aligned = false
        }
    };

    Gfx::Transform& meshTransform = mesh.transform();
    meshTransform.scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = glm::degrees(glm::eulerAngles(meshTransform.rotation));

    glm::vec2 mousePosition {};
    glm::vec2 lastMousePosition {};
    float mouseSensetivity {0.1f};

    Gfx::setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!Gfx::windowShouldClose())
    {
        Gfx::beginFrame();

        static float cameraSpeed = 0.05f * Gfx::deltaTime();
        if (Input::GetKeyDown(GLFW_KEY_W))
        {
            cam.position() += cameraSpeed * cam.front();
        }

        if (Input::GetKeyDown(GLFW_KEY_S))
        {
            cam.position() -= cameraSpeed * cam.front();
        }

        if (Input::GetKeyDown(GLFW_KEY_A))
        {
            cam.position() -= glm::normalize(glm::cross(cam.front(), cam.up())) * cameraSpeed;
        }

        if (Input::GetKeyDown(GLFW_KEY_D))
        {
            cam.position() += glm::normalize(glm::cross(cam.front(), cam.up())) * cameraSpeed;
        }

        if (Input::GetKeyDown(GLFW_KEY_SPACE))
        {
            cam.position() += cameraSpeed * cam.up();
        }

        if (Input::GetKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            cam.position() -= cameraSpeed * cam.up();
        }

        cam.update();

        Gfx::setShaderProgram(Gfx::defaultShaderProgram());

        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "view", cam.view());
        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "projection", cam.projection());

        Gfx::updateTextureData(texture);
        Gfx::drawIndexedGeometry(mesh.transform(), mesh.vertices(), mesh.indicies(), Gfx::defaultShaderProgram(), mesh.vertexBufferObject(), mesh.vertexArrayObject(), attributes);

        rotation.y += 100 * Gfx::deltaTime();
        if (rotation.y >= 360.0f)
        {
            rotation.y = 0;
        }
        meshTransform.rotation = glm::quat(glm::radians(rotation));

        mousePosition = Input::GetMousePosition();

        float xoffset = (mousePosition.x - lastMousePosition.x) * mouseSensetivity;
        float yoffset = (lastMousePosition.y - mousePosition.y) * mouseSensetivity;

        lastMousePosition = mousePosition;

        float& cameraYaw = cam.yaw();
        float& camperaPitch = cam.pitch();

        if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            cameraYaw += xoffset;
            camperaPitch += yoffset;
            camperaPitch = glm::clamp(camperaPitch, -89.0f, 89.0f);
        }

        float camPos[3] { cam.position().x, cam.position().y, cam.position().z};
        ImGui::Begin("Camera");
        if(ImGui::DragFloat3("Position", camPos, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()))
        {
            cam.position() = {camPos[0], camPos[1], camPos[2]};
        }
        ImGui::End();

        ImGui::Begin("Mouse");
        ImGui::SliderFloat("Sensetivity", &mouseSensetivity, 0.0f, 1.0f);
        ImGui::End();

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

        ImGui::Begin("UV");
        size_t index {0};
        for(Gfx::Vertex& vertex : mesh.vertices())
        {
            ImGui::SliderFloat(fmt::format("vert[{}].uv.x", index).c_str(), &vertex.uv.x, 0.0f, 1.0f);
            ImGui::SliderFloat(fmt::format("vert[{}].uv.y", index).c_str(), &vertex.uv.y, 0.0f, 1.0f);
            index++;
        }
        ImGui::End();

        Gfx::endFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Gfx::destroy();
    return 0;
}
