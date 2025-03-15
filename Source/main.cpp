
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fmt/format.h"
#include "Korelib.hpp"
#include "Gfx.hpp"

#include <cstddef>
#include <vector>

int main(int argc, char** argv)
{
    Gfx::initialize(800, 600, "Learn OpenGL", Gfx::WindowFlags::NONE);

    Gfx::Texture texture = Gfx::Texture::fromFile("Resources/Textures/brick.jpg");
    Gfx::Camera cam{45, 0.1f, 100};
    cam.unwrap(800, 600);

    Gfx::onWindowSizeChangedDelegate().bind([&cam](uint32_t w, uint32_t h)
    {
        cam.unwrap(w, h);
    });

    static Gfx::Mesh mesh{
        // vertex array [position, uv, color]
        {
            {{0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // top right
            {{0.5f, -0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // bottom left
            {{-0.5f,  0.5f, 0.0f},  {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // top left
        },
        // indicies array
        {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
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

        Gfx::setShaderProgram(Gfx::defaultShaderProgram());
        cam.update();

        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "view", cam.view());
        Gfx::setShaderMat4x4Value(Gfx::defaultShaderProgram(), "projection", cam.projection());

        Gfx::updateTextureData(texture);
        Gfx::drawIndexedGeometry(mesh.model(), mesh.vertices(), mesh.indicies(), Gfx::defaultShaderProgram(), mesh.vertexBufferObject(), mesh.vertexArrayObject(), attributes);

        Gfx::swap();
        Gfx::endFrame();
    }
    
    Gfx::destroy();
    return 0;
}
