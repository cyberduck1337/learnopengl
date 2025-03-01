#pragma once

#include <cstdint>
#include <string>

#include <functional>
#include <optional>

#include "Korelib.hpp"

class Gfx final : public korelib::StaticOnlyClass
{
    using WindowType = struct GLFWwindow*;
    using WindowReizeDelegate = korelib::Delegate<void, uint32_t, uint32_t>;
    
public:
    enum class WindowFlags : uint32_t
    {
        NONE = 0x00000000
    };

public:
    static void initialize(uint32_t width, uint32_t height, const std::string& title, WindowFlags flags);
    static void beginFrame();
    static bool windowShouldClose();
    static void setClearColor(float r, float g, float b, float a);
    static void clearBackground();
    static void swap();
    static void endFrame();
    static void destroy();

    static WindowReizeDelegate& onWindowSizeChangedDelegate()
    {
        return g_onWindowSizeChanged;
    }

private:
    static inline WindowType g_window { nullptr };
    static inline WindowReizeDelegate g_onWindowSizeChanged {};
};
