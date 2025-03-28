#include "SceneGraph.hpp"
#include "Korelib.hpp"

Entity::Entity(const std::string& name, const std::shared_ptr<Entity>& parent) : m_name(name), m_parent(parent)
{
}

Entity::Entity(const std::string& name) : Entity(name, nullptr)
{
}

const std::string& Entity::getName() const
{
    return m_name;
}

void Entity::setName(const std::string& name)
{
    m_name = name;
}

const std::shared_ptr<Entity>& Entity::getParent() const
{
    return m_parent;
}

void Entity::update()
{
    for (auto&& child : m_children)
    {
        child->update();
    }
}

GameObject::GameObject(const std::string& name, const std::shared_ptr<Entity>& parent) : Entity(name, parent)
{
    KORELIB_VERIFY_THROW(parent != nullptr, korelib::RuntimeException, "parent is null");
    KORELIB_VERIFY_THROW(parent->kind() == Entity::Kind::SCENE || parent->kind() == Entity::Kind::GAME_OBJECT, korelib::RuntimeException, "GameObject parent can be only Entity with type SCENE or GAME_OBJECT");
}

Component::Component(const std::string& name, const std::shared_ptr<Entity>& parent) : Entity(name, parent)
{
    KORELIB_VERIFY_THROW(parent != nullptr, korelib::RuntimeException, "parent is null");
    KORELIB_VERIFY_THROW(parent->kind() == Entity::Kind::GAME_OBJECT, korelib::RuntimeException, "Component parent can be only Entity with type GAME_OBJECT");
}

void Component::update()
{
}

std::shared_ptr<GameObject> Component::gameObject()
{
    return std::static_pointer_cast<GameObject>(getParent());
}

std::shared_ptr<Scene> Scene::create(const std::string& name)
{
    return std::make_shared<Scene>(name);
}

Scene::Scene(const std::string& name) : Entity(name, nullptr)
{
}

std::shared_ptr<GameObject> Scene::addGameObject(const std::string& name, const glm::vec3& position, std::shared_ptr<GameObject> parent)
{
    std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name, parent == nullptr ? std::static_pointer_cast<Entity>(shared_from_this()) : parent);
    go->m_transform.position = position;
    m_children.emplace_back(go);
    return std::static_pointer_cast<GameObject>(go);
}

std::shared_ptr<GameObject> Scene::addGameObject(const std::string& name, const glm::vec3& position)
{
    return addGameObject(name, position, nullptr);
}
