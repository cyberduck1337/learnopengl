#include "Texture.hpp"
#include "Gfx.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::load()
{
    uint8_t* data = stbi_load(getPath().string().c_str(), &m_width, &m_height, &m_channels, 0);
    KORELIB_VERIFY_THROW(data != nullptr, korelib::RuntimeException, fmt::format("Failed to load texture '{}'", getPath().string()));

    m_textureId = Gfx::textureFromData(data, m_width, m_height);
    stbi_image_free(data);
}
