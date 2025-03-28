#pragma once

#include "ctti/type_id.hpp"
#include "Korelib.hpp"
#include "Gfx.hpp"
#include "glm/glm.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Entity : public std::enable_shared_from_this<Entity>
{
public:
    enum class Kind : uint8_t
    {
        COMPONENT,
        GAME_OBJECT,
        SCENE
    };

    virtual ~Entity() = default;

    virtual constexpr Kind kind() const = 0;
    virtual void update();

    const std::string& getName() const;
    void setName(const std::string& name);

    const std::shared_ptr<Entity>& getParent() const;

protected:
    Entity(const std::string& name, const std::shared_ptr<Entity>& parent);
    Entity(const std::string& name);

    void refreshChildIndices();

protected:
    std::list<std::shared_ptr<Entity>> m_children;

private:
    std::string m_name;
    std::shared_ptr<Entity> m_parent;
};

class GameObject final : public Entity
{
public:
    virtual constexpr Kind kind() const final override
    {
        return Kind::GAME_OBJECT;
    }

    GameObject(const std::string& name, const std::shared_ptr<Entity>& parent);

    template<typename T, typename... TArgs>
    std::shared_ptr<T> addComponent(TArgs&&... args) requires(std::derived_from<T, class Component>)
    {
        ctti::unnamed_type_id_t key = ctti::unnamed_type_id<T>();
        std::shared_ptr<T> component = std::make_shared<T>(shared_from_this(), std::forward<TArgs>(args)...);

        m_children.emplace_back(component);
        const std::size_t componentIndex = m_children.size() - 1;

        if(m_componentTypeToIndicesMap.contains(key))
        {
            std::vector<std::size_t>& componentsList = m_componentTypeToIndicesMap.at(key);
            componentsList.emplace_back(componentIndex);

            return component;
        }

        std::vector<std::size_t> componentList{ componentIndex };
        m_componentTypeToIndicesMap.insert(std::make_pair(key, std::move(componentList)));
        return component;
    }

    template<typename T>
    std::vector<std::shared_ptr<T>> getComponents() requires(std::derived_from<T, class Component>)
    {
        std::vector<std::shared_ptr<T>> result{};

        constexpr ctti::unnamed_type_id_t typeId = ctti::unnamed_type_id<T>();
        if (m_componentTypeToIndicesMap.contains(typeId))
        {
            const std::vector<size_t>& indices = m_componentTypeToIndicesMap.at(typeId);
            if (!indices.empty())
            {
                for (size_t index : indices)
                {
                    auto it = m_children.begin();
                    std::advance(it, index);

                    std::shared_ptr<Entity> component = *it;
                    KORELIB_VERIFY_THROW(component->kind() == Entity::Kind::COMPONENT, korelib::RuntimeException, "Unexpected Entity type");
                    result.emplace_back(std::move(std::static_pointer_cast<T>(component)));
                }
            }
        }

        return result;
    }

    template<typename T>
    std::shared_ptr<T> getComponent() requires(std::derived_from<T, class Component>)
    {
        std::vector<std::shared_ptr<T>> components = getComponents<T>();
        if (!components.empty())
        {
            return components[0];
        }

        return nullptr;
    }

public:
    Gfx::Transform m_transform;

protected:
    std::unordered_map<ctti::unnamed_type_id_t, std::vector<size_t>> m_componentTypeToIndicesMap;
};

class Component : public Entity
{
public:
    virtual constexpr Kind kind() const final override
    {
        return Kind::COMPONENT;
    }

    virtual void update() override;

protected:
    Component(const std::string& name, const std::shared_ptr<Entity>& parent);
};

class Scene final : public Entity
{
public:
    virtual constexpr Kind kind() const final override
    {
        return Kind::SCENE;
    }

    static std::shared_ptr<Scene> create(const std::string& name);

    Scene(const std::string& name);
    std::shared_ptr<GameObject> addGameObject(const std::string& name, const glm::vec3& position, std::shared_ptr<GameObject> parent);
    std::shared_ptr<GameObject> addGameObject(const std::string& name, const glm::vec3& position);
};
