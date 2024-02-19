#pragma once

#include <array>
#include <bitset>
#include <numeric>
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

    template <typename Component>
    void remove() noexcept
    {
        ptr<Component>()->~Component();
        disable<Component>();
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

    template <typename Component>
    void enable() noexcept
    {
        components_[index<Component>()] = true;
    }

    template <typename Component>
    void disable() noexcept
    {
        components_[index<Component>()] = false;
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

template <typename Entity>
using EntityStorage = std::vector<Entity>;

} // namespace engine::ecs
