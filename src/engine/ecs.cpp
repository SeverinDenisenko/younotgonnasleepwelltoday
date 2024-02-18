#include "ecs.hpp"

namespace engine::ecs
{
void ECS::finishRegistration()
{
    is_component_registration_allowed_ = false;

    for (const ComponentDescriptor& desctiptor : component_descriptors_) {
        component_offsets_[desctiptor.id] = entity_size_;
        entity_size_ += desctiptor.size;
        component_count_++;
    }

    assert(entity_size_ != 0 && "You have to register atleast one component with non-zero size");
}

EntityBuilder ECS::getEntityBuilder()
{
    assert(!is_storage_got_out_ && "You already got storage");
    assert(!is_component_registration_allowed_ && "You have to finish component registration before getting entity storage");

    EntityBuilder builder{*this};

    is_storage_got_out_ = true;

    return builder;
}

EntityStorage ECS::getEntityStorage()
{
    assert(!is_builder_got_out_ && "You already got builder");
    assert(!is_component_registration_allowed_ && "You have to finish component registration before getting entity builder");

    EntityStorage builder{MAX_ENTITIES_COUNT, entity_size_, *this};

    return builder;
}

component_id_t ECS::getUniqueId()
{
    static component_id_t id{0};
    return id++;
}

entity_id_t EntityStorage::createEntity(EntityDescriptor desctiptor)
{
    descriptors_[current_entity_] = desctiptor;
    return current_entity_++;
}

EntityStorage::EntityStorage(size_t entity_count, size_t entity_size, ECS& ecs)
    : entity_count_(entity_count)
    , entity_size_(entity_size)
    , entities_(entity_count_ * entity_size_)
    , descriptors_(entity_count_)
    , ecs_(ecs)
{
}

EntityBuilder& EntityBuilder::clear()
{
    components_.reset();
    return *this;
}

EntityDescriptor EntityBuilder::build()
{
    return components_;
}

EntityBuilder::EntityBuilder(ECS& ecs)
    : ecs_(ecs)
{
}

} // namespace engine::ecs
