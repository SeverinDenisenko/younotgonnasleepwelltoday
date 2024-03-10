#pragma once

#include <filesystem>
#include <unordered_map>

#include "defines.hpp"

namespace engine {

using Texture = Texture2D;
using Music   = Music;

class Filesystem {
public:
    Filesystem() noexcept;

    string resolve(string relative) noexcept;

private:
    std::filesystem::path root_;
};

template <typename Alias>
class TextureHolder {
public:
    TextureHolder(Filesystem& fs)
        : fs_(fs)
    {
    }

    Texture& load(string name, Alias alias) noexcept
    {
        Texture res = LoadTexture(fs_.resolve(name).c_str());
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
    Filesystem& fs_;
};

template <typename Alias>
class AudioHolder {
public:
    AudioHolder(Filesystem& fs)
        : fs_(fs)
    {
    }

    Music& load(string name, Alias alias) noexcept
    {
        Music res = LoadMusicStream(fs_.resolve(name).c_str());
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
    Filesystem& fs_;
};

} // namespace engine
