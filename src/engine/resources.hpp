#pragma once

#include <unordered_map>

#include "defines.hpp"

namespace engine {

using Texture = Texture2D;
using Music   = Music;

template <typename Alias>
class TextureHolder {
public:
    Texture& load(cstr name, Alias alias) noexcept
    {
        Texture res = LoadTexture(name);
        textures_.emplace(alias, res);

        return get(alias);
    }

    Texture& get(Alias alias) noexcept
    {
        return textures_[alias];
    }

    void unload(Alias alias) noexcept
    {
        UnloadTexture(textures_[alias]);
        textures_.erase(alias);
    }

    ~TextureHolder()
    {
        for (auto& [alias, texture] : textures_) {
            UnloadTexture(texture);
        }
    }

private:
    std::unordered_map<Alias, Texture> textures_;
};

template <typename Alias>
class AudioHolder {
public:
    Music& load(cstr name, Alias alias) noexcept
    {
        Music res = LoadMusicStream(name);
        music_.emplace(alias, res);

        return get(alias);
    }

    Music& get(Alias alias) noexcept
    {
        return music_[alias];
    }

    void unload(Alias alias) noexcept
    {
        UnloadMusicStream(music_[alias]);
        music_.erase(alias);
    }

    ~AudioHolder()
    {
        for (auto& [alias, music] : music_) {
            UnloadMusicStream(music);
        }
    }

private:
    std::unordered_map<Alias, Music> music_;
};

} // namespace engine
