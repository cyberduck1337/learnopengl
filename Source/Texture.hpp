#pragma once

#include "Gfx.hpp"
#include "Resource.hpp"

class Texture : public Resource
{
public:
    template<korelib::concepts::PathLike PathType>
    constexpr Texture(PathType&& path, StorageType storageType) noexcept : Resource{std::forward<PathType>(path), storageType}
    {
    }

    constexpr Gfx::TextureIdType getTextureId() const noexcept
    {
        return m_textureId;
    }

    constexpr int32_t getWidth() const noexcept
    {
        return m_width;
    }

    constexpr int32_t getHeight() const noexcept
    {
        return m_height;
    }

    constexpr int32_t getNumChannels() const noexcept
    {
        return m_channels;
    }

    void load();

private:
    Gfx::TextureIdType m_textureId {0};
    int32_t m_width {0};
    int32_t m_height {0};
    int32_t m_channels {0};
};
