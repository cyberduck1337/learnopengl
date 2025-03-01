#include "fmt/format.h"
#include "Korelib.hpp"
#include "Gfx.hpp"

int main(int argc, char** argv)
{
    Gfx::initialize(800, 600, "Learn OpenGL", Gfx::WindowFlags::NONE);
    Gfx::setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!Gfx::windowShouldClose())
    {
        Gfx::beginFrame();
        Gfx::endFrame();
    }
    
    Gfx::destroy();
    return 0;
}
