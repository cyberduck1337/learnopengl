#include "Camera.hpp"
#include "Gfx.hpp"

Camera::Camera(const std::shared_ptr<Entity>& parent, float fov, float near, float far) : Component("Camera", parent), m_fov(fov), m_near(near), m_far(far)
{
}

float& Camera::fov()
{
    return m_fov;
}

float& Camera::near()
{
    return m_near;
}

float& Camera::far()
{
    return m_far;
}

const glm::mat4& Camera::view() const
{
    return m_view;
}

const glm::mat4& Camera::projection() const
{
    return m_projection;
}

void Camera::update()
{
    const glm::uvec2 windowSize = Gfx::getWindowSize();
    m_projection = glm::perspective(glm::radians(m_fov), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), m_near, m_far);

    const std::shared_ptr<GameObject>& parent = std::static_pointer_cast<GameObject>(getParent());
    Gfx::Transform& transform = parent->m_transform;
    m_view = glm::lookAt(transform.position, transform.position + transform.front(), transform.up());
}
