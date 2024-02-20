#pragma once

#include <array>
#include <bitset>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

#include "defines.hpp"

namespace engine::ecs
{

namespace utils
{
namespace detail
{
template <typename T, typename... Ts>
struct Index;

template <typename T, typename... Ts>
struct Index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {
};

template <typename T, typename U, typename... Ts>
struct Index<T, U, Ts...> : std::integral_constant<std::size_t, 1 + Index<T, Ts...>::value> {
};
} // namespace detail

/*
 * Returns index of T in variadic template parameter Ts
 */
template <typename T, typename... Ts>
constexpr std::size_t Index = detail::Index<T, Ts...>::value;

/*
 * Returns count of template parameters in Ts
 */
template <typename... Ts>
constexpr std::size_t Count = sizeof...(Ts);

/*
 * Returns summary size of all template parameters Ts
 */
template <typename... Ts>
constexpr std::size_t Size = (sizeof(Ts) + ...);

/*
 * Returns sizes of all template parameters Ts
 */
template <typename... Ts>
constexpr std::array<size_t, Count<Ts...>> Sizes = {sizeof(Ts)...};
} // namespace utils

template <typename... Components>
class Entity
{
public:
    Entity() = default;

    static constexpr size_t size() noexcept
    {
        return size_;
    }

    static constexpr size_t count() noexcept
    {
        return count_;
    }

    template <typename Component>
    static constexpr size_t index() noexcept
    {
        return utils::Index<Component, Components...>;
    }

    template <typename Component>
    static constexpr size_t size() noexcept
    {
        return sizes_[index<Component>()];
    }

    template <typename... RequaredComponents>
    bool contains() const noexcept
    {
        return (components_[index<RequaredComponents>()] && ...);
    }

    template <typename Component, typename... Args>
    Component& add(Args... args) noexcept
    {
        new (ptr<Component>()) Component(std::forward<Args>(args)...);
        enable<Component>();
        return *ptr<Component>();
    }

    template <typename... RequaredComponents>
    void enable() noexcept
    {
        (components_[index<RequaredComponents>()] = ... = true);
    }

    template <typename... RequaredComponents>
    void disable() noexcept
    {
        (components_[index<RequaredComponents>()] = ... = false);
    }

    void destroy() noexcept
    {
        disable<Components...>();
    }

    template <typename Component>
    Component& get() noexcept
    {
        return *ptr<Component>();
    }

private:
    template <typename Component>
    static constexpr size_t offset() noexcept
    {
        return std::accumulate(sizes_.begin(), sizes_.begin() + index<Component>(), 0);
    }

    template <typename Component>
    Component* ptr() noexcept
    {
        return reinterpret_cast<Component*>(components_storage_.data() + offset<Component>());
    }

    static constexpr size_t count_ = utils::Count<Components...>;
    static constexpr size_t size_  = utils::Size<Components...>;
    static constexpr auto sizes_   = utils::Sizes<Components...>;

    template <size_t I>
    using component_index_t = std::bitset<I>;

    template <size_t J>
    using component_storage_t = std::array<byte, J>;

    component_index_t<count_> components_{0};
    component_storage_t<size_> components_storage_;
};

using EntityId = size_t;

template <typename Entity>
class EntityStorage
{
public:
    template <typename... RequaredComponents>
    using ComponentsRefs = std::tuple<RequaredComponents&...>;

    EntityId create() noexcept
    {
        if (dead_.size() > 0) {
            EntityId result = dead_.back();
            dead_.pop_back();
            return result;
        } else {
            EntityId result = entities_.size();
            entities_.emplace_back();
            return result;
        }
    }

    EntityId size() const noexcept
    {
        return entities_.size();
    }

    EntityId active() const noexcept
    {
        return entities_.size() - dead_.size();
    }

    Entity& get(EntityId i) noexcept
    {
        return entities_[i];
    }

    void remove(EntityId i) noexcept
    {
        if (std::find(dead_.begin(), dead_.end(), i) == dead_.end()) {
            entities_[i].destroy();
            dead_.push_back(i);
        }
    }

    template <typename... RequaredComponents>
    class Iterator
    {
    public:
        Iterator(EntityStorage& storage) noexcept
            : storage_(storage)
        {
            while (!end() && !good()) {
                ++curr_;
            }
        }

        ComponentsRefs<RequaredComponents...> operator*() const noexcept
        {
            return ComponentsRefs<RequaredComponents...>(storage_.get(curr_).template get<RequaredComponents>()...);
        }

        Iterator& operator++() noexcept
        {
            do {
                ++curr_;
            } while (!end() && !good());

            return *this;
        }

        operator bool() const noexcept
        {
            return !end();
        }

        EntityId id() const noexcept
        {
            return curr_;
        }

    private:
        bool good() noexcept
        {
            return storage_.get(curr_).template contains<RequaredComponents...>();
        }

        bool end() const noexcept
        {
            return curr_ >= storage_.size();
        }

        EntityId curr_{0};
        EntityStorage& storage_;
    };

    template <typename... RequaredComponents>
    Iterator<RequaredComponents...> iterator() noexcept
    {
        return Iterator<RequaredComponents...>(*this);
    }

    template <typename... RequaredComponents>
    std::tuple<RequaredComponents&...> get() noexcept
    {
        for (Entity& e : entities_) {
            if (e.template contains<RequaredComponents...>()) {
                return ComponentsRefs<RequaredComponents...>(e.template get<RequaredComponents>()...);
            }
        }

        assert(false);
        __builtin_unreachable();
    }

private:
    std::vector<Entity> entities_;
    std::vector<EntityId> dead_;
};

template <typename Entity>
class EntityBuilder
{
public:
    EntityBuilder(EntityStorage<Entity>& storage)
        : storage_(storage)
    {
    }

    EntityBuilder& create() noexcept
    {
        current_ = storage_.create();
        return *this;
    }

    template <typename Component, typename... Args>
    EntityBuilder& with(Args... args) noexcept
    {
        storage_.get(current_).template add<Component>(std::forward<Args>(args)...);
        return *this;
    }

    void build() noexcept {}

private:
    size_t current_{0};
    EntityStorage<Entity>& storage_;
};

template <typename Entity>
class System
{
public:
    using Storage = EntityStorage<Entity>;

    virtual void setup(Storage&) noexcept {};
    virtual void update(Storage&) noexcept {};

    virtual ~System() = default;
};

template <typename System>
class SystemManager
{
public:
    void add(uptr<System> system) noexcept
    {
        system->setup(storage_);
        systems_.push_back(std::move(system));
    }

    void update()
    {
        for (auto& s : systems_) {
            s->update(storage_);
        }
    }

private:
    std::vector<uptr<System>> systems_;
    System::Storage storage_;
};

} // namespace engine::ecs
