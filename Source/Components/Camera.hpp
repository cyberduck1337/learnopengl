#pragma once

#include "SceneGraph.hpp"
#include "glm/glm.hpp"

class Camera : public Component
{
public:
    Camera(const std::shared_ptr<Entity>& parent, float fov, float near, float far);

    void update() override;

    float& fov();
    float& near();
    float& far();

    const glm::mat4& view() const;
    const glm::mat4& projection() const;

private:
    float m_fov;
    float m_near;
    float m_far;

    glm::mat4 m_view;
    glm::mat4 m_projection;
};
