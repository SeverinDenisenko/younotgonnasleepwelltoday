#include "fmt/format.h"
// #include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"
#include "engine/profiling.hpp"
#include "engine/resources.hpp"

#include <cmath>

#include "config.hpp"

namespace impl {

namespace components {

struct Camera {
    Camera(engine::f32 zoom)
        : zoom(zoom)
    {
    }

    Camera() = default;

    engine::f32 zoom{1.0f};
};

struct Transform {
    Transform(engine::f32 x, engine::f32 y)
        : pos(x, y)
    {
    }

    Transform() = default;

    engine::vec2 pos{0.0f, 0.0f};
    engine::vec2 scale{1.0f, 1.0f};
    engine::vec2 origin{0.0f, 0.0f};
    engine::f32 rot{0.0f};
};

class TransformBuilder {
public:
    TransformBuilder& create()
    {
        t_ = Transform();
        return *this;
    }

    TransformBuilder& position(engine::f32 x, engine::f32 y)
    {
        t_.pos.x = x;
        t_.pos.y = y;
        return *this;
    }

    TransformBuilder& scale(engine::f32 x, engine::f32 y)
    {
        t_.scale.x = x;
        t_.scale.y = y;
        return *this;
    }

    TransformBuilder& origin(engine::f32 x, engine::f32 y)
    {
        t_.origin.x = x;
        t_.origin.y = y;
        return *this;
    }

    TransformBuilder& rotation(engine::f32 r)
    {
        t_.rot = r;
        return *this;
    }

    Transform build()
    {
        return t_;
    }

private:
    Transform t_;
};

struct Text {
    Text(engine::cstr text, engine::f32 size, engine::f32 hspacing, engine::f32 wspacing)
        : text(text)
        , size(size)
        , hspacing(hspacing)
        , wspacing(wspacing)
    {
    }

    engine::cstr text{""};
    engine::f32 size{0.0f};
    engine::f32 hspacing{0.0f};
    engine::f32 wspacing{0.0f};
};

struct Color {
    Color(::Color color)
        : color(color)
    {
    }

    Color(engine::u8 r, engine::u8 g, engine::u8 b)
        : color{r, g, b, 255}
    {
    }

    ::Color color{};
};

struct Sprite {
    engine::Texture texture;
    engine::vec2 pos;
    engine::vec2 size;
};

class SpriteBuilder {
public:
    SpriteBuilder& create()
    {
        s_ = Sprite();
        return *this;
    }

    SpriteBuilder& texture(engine::Texture texture)
    {
        s_.texture = texture;
        return *this;
    }

    SpriteBuilder& position(engine::f32 x, engine::f32 y)
    {
        s_.pos.x = x;
        s_.pos.y = y;
        return *this;
    }

    SpriteBuilder& size(engine::f32 x, engine::f32 y)
    {
        s_.size.x = x;
        s_.size.y = y;
        return *this;
    }

    Sprite build()
    {
        return s_;
    }

private:
    Sprite s_;
};

struct Audio {
    engine::Music music{};
    bool playing{false};
};

struct Player {};

struct Flags {
    bool ui{false};
    bool cell{false};
};

} // namespace components

namespace game_preferenses {

static const engine::f32 grid_size = 10.0;
static const engine::f32 cell_size = RENDER_WIDTH / grid_size;

}; // namespace game_preferenses

using Entity = engine::ecs::Entity<
    components::Camera,
    components::Transform,
    components::Color,
    components::Text,
    components::Player,
    components::Sprite,
    components::Audio,
    components::Flags>;
using EntityStorage = engine::ecs::EntityStorage<Entity>;
using EntityBuilder = engine::ecs::EntityBuilder<Entity>;
using System        = engine::ecs::System<Entity>;
using SystemManager = engine::ecs::SystemManager<System>;
using TextureHolder = engine::TextureHolder<engine::string>;
using AudioHolder   = engine::AudioHolder<engine::string>;

class DebugSystem : public System {
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Flags>(components::Flags{.ui = true})
            .with<components::Text>("Placeholder", 15.0, 1.0, 15.0)
            .with<components::Color>(RED)
            .with<components::Transform>(1.0f, 1.0f)
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        fps_ = fmt::format(
            "FPS: {}\n"
            "Active: {}\n"
            "All: {}\n"
            "Memory: {:.2} MB\n",
            GetFPS(),
            storage.active(),
            storage.size(),
            engine::f32(Entity::size() * storage.size()) / 1024.0f / 1024.0f);

        const auto& [text] = storage.get<components::Text>();
        text.text          = fps_.data();
    }

private:
    std::string fps_;
};

class PlayerSystem : public System {
public:
    PlayerSystem(TextureHolder& holder)
    {
        planet = holder.load("Lava.png", "player");
    }

    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Flags>(components::Flags{.ui = false})
            .with<components::Player>()
            .with<components::Color>(WHITE)
            .with<components::Transform>(components::TransformBuilder()
                                             .create()
                                             .scale(game_preferenses::cell_size, game_preferenses::cell_size)
                                             .origin(1.5f, 1.5f)
                                             .build())
            .with<components::Sprite>(components::SpriteBuilder()
                                          .create()
                                          .texture(planet)
                                          .position(0.0f, 0.0f)
                                          .size(planet.width, planet.height)
                                          .build())
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        engine::f32 dt    = GetFrameTime();
        engine::f32 speed = 10.0f;

        const auto& [player, ptransform] = storage.get<components::Player, components::Transform>();

        if (IsKeyDown(KEY_W)) {
            ptransform.pos.y -= dt * speed;
        }
        if (IsKeyDown(KEY_S)) {
            ptransform.pos.y += dt * speed;
        }
        if (IsKeyDown(KEY_A)) {
            ptransform.pos.x -= dt * speed;
        }
        if (IsKeyDown(KEY_D)) {
            ptransform.pos.x += dt * speed;
        }
    }

private:
    engine::Texture planet;
};

class CellSystem : public System {
public:
    CellSystem(TextureHolder& holder)
    {
        planet = holder.load("Terran.png", "planet");
    }

    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        std::function<engine::f32(engine::f32)> coord = [](engine::f32 i) {
            return i * game_preferenses::cell_size - game_preferenses::grid_size * game_preferenses::cell_size / 2.0;
        };

        for (engine::u32 i = 0; i < game_preferenses::grid_size; ++i) {
            for (engine::u32 j = 0; j < game_preferenses::grid_size; ++j) {
                engine::f32 x = coord(i);
                engine::f32 y = coord(j);

                builder.create()
                    .with<components::Flags>(components::Flags{.ui = false, .cell = true})
                    .with<components::Color>(WHITE)
                    .with<components::Transform>(components::TransformBuilder()
                                                     .create()
                                                     .position(x, y)
                                                     .scale(game_preferenses::cell_size, game_preferenses::cell_size)
                                                     .build())
                    .with<components::Sprite>(components::SpriteBuilder()
                                                  .create()
                                                  .texture(planet)
                                                  .position(0.0f, 0.0f)
                                                  .size(planet.width, planet.height)
                                                  .build())
                    .build();
            }
        }
    }

    void update(Storage&) noexcept override {}

private:
    engine::Texture planet;
};

class RenderSystem : public System {
public:
    RenderSystem(engine::u32 w, engine::u32 h, engine::f32 view)
        : w_(w)
        , h_(h)
        , view_(view)
    {
    }

    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Camera>(h_ / 2.0f / view_)
            .with<components::Transform>(components::TransformBuilder().create().origin(w_ / 4.0f, h_ / 4.0f).build())
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        PROFILE_FUNCTION();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        texures(storage, true);
        text(storage, true);

        const auto& [camera, transform] = storage.get<components::Camera, components::Transform>();
        BeginMode2D((Camera2D){
            .target   = (Vector2){transform.pos.x, transform.pos.y},
            .offset   = (Vector2){transform.origin.x, transform.origin.y},
            .rotation = transform.rot,
            .zoom     = camera.zoom});

        texures(storage, false);
        text(storage, false);

        EndMode2D();
        EndDrawing();
    }

private:
    void text(Storage& storage, bool ui)
    {
        PROFILE_FUNCTION();

        auto text_iter =
            storage.iterator<components::Text, components::Transform, components::Color, components::Flags>();

        while (text_iter) {
            const auto& [text, transform, color, flags] = *text_iter;

            if (flags.ui == ui) {
                SetTextLineSpacing(text.wspacing);
                DrawTextPro(
                    GetFontDefault(),
                    text.text,
                    (Vector2){transform.pos.x, transform.pos.y},
                    (Vector2){transform.origin.x, transform.origin.y},
                    transform.rot,
                    text.size,
                    text.hspacing,
                    color.color);
            }

            ++text_iter;
        }
    }

    void texures(Storage& storage, bool ui)
    {
        PROFILE_FUNCTION();

        auto texures_iter =
            storage.iterator<components::Sprite, components::Transform, components::Color, components::Flags>();

        while (texures_iter) {
            const auto& [sprite, transform, color, flags] = *texures_iter;

            if (flags.ui == ui) {
                DrawTexturePro(
                    sprite.texture,
                    (Rectangle){sprite.pos.x, sprite.pos.y, sprite.size.x, sprite.size.y},
                    (Rectangle){transform.pos.x, transform.pos.y, transform.scale.x, transform.scale.y},
                    (Vector2){transform.origin.x, transform.origin.y},
                    transform.rot,
                    color.color);
            }

            ++texures_iter;
        }
    }

    engine::u32 w_{0};
    engine::u32 h_{0};
    engine::f32 view_{0};
};

class AudioSystem : public System {
public:
    AudioSystem(AudioHolder& holder)
    {
        music_ = holder.load("music.mp3", "piano");
    }

    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create().with<components::Audio>(components::Audio{music_, true}).build();
    }

    void update(Storage& storage) noexcept override
    {
        PROFILE_FUNCTION();

        auto audio_iter = storage.iterator<components::Audio>();

        while (audio_iter) {
            const auto& [audio] = *audio_iter;

            if (audio.playing) {
                PlayMusicStream(audio.music);
                UpdateMusicStream(audio.music);
            }
            else {
                StopMusicStream(audio.music);
            }

            ++audio_iter;
        }
    }

private:
    engine::Music music_;
};

class Game : public engine::Game {
public:
    Game()
        : engine::Game("You Not Gonna Sleep Well Today")
    {
    }

    void setup() noexcept override
    {
        PROFILE_FUNCTION();

        engine::Game::setup();

        manager_.add(std::make_unique<RenderSystem>(width(), height(), RENDER_WIDTH));
        manager_.add(std::make_unique<CellSystem>(textures_));
        manager_.add(std::make_unique<PlayerSystem>(textures_));
        manager_.add(std::make_unique<AudioSystem>(audio_));
        manager_.add(std::make_unique<DebugSystem>());
    }

    void update() noexcept override
    {
        PROFILE_FUNCTION();

        manager_.update();

        if (IsKeyPressed(KEY_ESCAPE)) {
            exit();
        }
    }

    void shutdown() noexcept override
    {
        PROFILE_FUNCTION();

        engine::Game::shutdown();
    }

private:
    engine::Filesystem fs_{RESOURCES_PATH};
    SystemManager manager_;
    TextureHolder textures_{fs_};
    AudioHolder audio_{fs_};
};
} // namespace impl


int main(void)
{
    engine::uptr<engine::IGame> game    = std::make_unique<impl::Game>();
    engine::uptr<engine::Runner> runner = std::make_unique<engine::Runner>(std::move(game));
    runner->run();

    return 0;
}
