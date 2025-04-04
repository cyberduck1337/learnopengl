#pragma once

#include "Korelib.hpp"

#include <filesystem>
#include <concepts>
#include <string>

class Resource
{
public:
    enum class StorageType : uint8_t
    {
        LOCAL,
        ARCHIVE
    };

public:
    template<korelib::concepts::PathLike PathType>
    constexpr Resource(PathType&& path, StorageType storageType) noexcept : m_path{std::forward<PathType>(path)}, m_storageType{storageType}
    {
    }

    constexpr virtual ~Resource() noexcept = default;

    constexpr const std::filesystem::path& getPath() const noexcept
    {
        return m_path;
    }

    constexpr StorageType getStorageType() const noexcept
    {
        return m_storageType;
    }

private:
    std::filesystem::path m_path;
    StorageType m_storageType;
};
