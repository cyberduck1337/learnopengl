
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "fmt/format.h"
#include "Korelib.hpp"
#include "Gfx.hpp"
#include "glm/glm.hpp"

#include <cstddef>
#include <vector>


int main(int argc, char** argv)
{
    Gfx::initialize(800, 600, "Learn OpenGL", Gfx::WindowFlags::NONE);

    static const Gfx::Mesh mesh{
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

    Gfx::Texture texture = Gfx::Texture::fromFile("Resources/Textures/brick.jpg");

    Gfx::setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!Gfx::windowShouldClose())
    {
        Gfx::beginFrame();
        Gfx::clearBackground();

        Gfx::setShaderProgram(Gfx::defaultShaderProgram());
        Gfx::updateTextureData(texture);
        Gfx::drawIndexedGeometry(mesh.vertices(), mesh.indicies(), Gfx::defaultShaderProgram(), mesh.vertexBufferObject(), mesh.vertexArrayObject(), attributes);

        Gfx::swap();
        Gfx::endFrame();
    }
    
    Gfx::destroy();
    return 0;
}
