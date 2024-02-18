#pragma once

#include <array>
#include <bitset>
#include <numeric>
#include <typeinfo>
#include <vector>

#include "defines.hpp"

namespace engine::ecs
{
// https://youtu.be/QAmtgvwHInM?si=agcY-xAfZmwu9GIG

constexpr size_t MAX_COMPONENT_COUNT = 128;
constexpr size_t MAX_ENTITIES_COUNT  = 1 << 15;

using EntityComponentOffsets = std::array<size_t, MAX_COMPONENT_COUNT>;
using EntityDescriptor       = std::bitset<MAX_COMPONENT_COUNT>;
using entity_id_t            = size_t;
using component_id_t         = size_t;

class EntityStorage;
class EntityBuilder;

class ECS
{
public:
    template <typename T>
    void registerComponent();

    void finishRegistration();

    EntityBuilder getEntityBuilder();

    EntityStorage getEntityStorage();

    template <typename T>
    component_id_t getComponentId();

    template <typename T>
    size_t getComponentOffset();

private:
    struct ComponentDescriptor {
        component_id_t id;
        size_t size;
        cstr name;
    };

    component_id_t getUniqueId();

    template <typename T>
    component_id_t getUniqueComponentId();

    size_t entity_size_{0};
    size_t component_count_{0};
    EntityComponentOffsets component_offsets_{};

    std::vector<ComponentDescriptor> component_descriptors_{};
    bool is_component_registration_allowed_{true};
    bool is_storage_got_out_{false};
    bool is_builder_got_out_{false};
};

template <typename... Args>
class ISystem
{
public:
    EntityDescriptor requarement() const noexcept = 0;
    void setup(Args... args) noexcept             = 0;
    void update(Args... args) noexcept            = 0;
};

class EntityStorage
{
    friend class ECS;

public:
    entity_id_t createEntity(EntityDescriptor desctiptor);

    template <typename T>
    T& getComponent(entity_id_t id);

private:
    size_t entity_count_{0};
    size_t entity_size_{0};
    std::vector<std::byte> entities_;
    std::vector<EntityDescriptor> descriptors_;
    ECS& ecs_;

    entity_id_t current_entity_{0};

    EntityStorage(size_t entity_count, size_t entity_size, ECS& ecs);
};

class EntityBuilder
{
    friend class ECS;

public:
    EntityBuilder& clear();

    template <typename T>
    EntityBuilder& withComponent();

    EntityDescriptor build();

private:
    EntityDescriptor components_{0};
    ECS& ecs_;

    EntityBuilder(ECS& ecs);
};

template <typename T>
void ECS::registerComponent()
{
    assert(is_component_registration_allowed_ && "Component registration does not allowed after game is started");

    ComponentDescriptor desctiptor;

    desctiptor.id   = getUniqueComponentId<T>();
    desctiptor.size = sizeof(T);
    desctiptor.name = typeid(T).name();

    component_descriptors_.push_back(desctiptor);
}

template <typename T>
component_id_t ECS::getComponentId()
{
    assert(!is_component_registration_allowed_ && "You have to finish component registration before getting components id");

    component_id_t id = getUniqueComponentId<T>();

    assert(id < component_count_ && "You tried to get id of unregistred component");

    return id;
}

template <typename T>
size_t ECS::getComponentOffset()
{
    component_id_t id = getComponentId<T>();

    return component_offsets_[id];
}

template <typename T>
component_id_t ECS::getUniqueComponentId()
{
    static component_id_t component_id{getUniqueId()};
    return component_id;
}

template <typename T>
T& EntityStorage::getComponent(entity_id_t id)
{
    return *reinterpret_cast<T*>(entities_.data() + id * entity_size_ + ecs_.getComponentOffset<T>());
}


template <typename T>
EntityBuilder& EntityBuilder::withComponent()
{
    components_[ecs_.getComponentId<T>()] = true;
    return *this;
}

} // namespace engine::ecs
