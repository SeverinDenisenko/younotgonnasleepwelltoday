#pragma once

#include <unordered_map>

#include "defines.hpp"

namespace engine {

using Texture = Texture2D;

template <typename Alias>
class ResourceHolder {
public:
    Texture load(cstr name, Alias alias) noexcept
    {
        Texture res = LoadTexture(name);
        textures_.emplace(alias, res);

        return res;
    }

    Texture get(Alias alias) const noexcept
    {
        return textures_.at(alias);
    }

    Texture unload(Alias alias) noexcept
    {
        UnloadTexture(textures_[alias]);
        textures_.erase(alias);
    }

    ~ResourceHolder()
    {
        for (auto& [alias, texture] : textures_) {
            UnloadTexture(texture);
        }
    }

private:
    std::unordered_map<Alias, Texture> textures_;
};

} // namespace engine
