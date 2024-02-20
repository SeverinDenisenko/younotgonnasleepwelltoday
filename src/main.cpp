#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"

#include <iostream>

namespace impl
{

namespace components
{

struct Camera {
    Camera(engine::f32 x, engine::f32 y)
        : pos(x, y)
    {
    }

    engine::vec2 pos{};
    engine::f32 zoom{1};
    engine::f32 rot{0};
};

struct Transform {
    Transform(engine::f32 x, engine::f32 y)
        : pos(x, y)
        , scale(1.0f, 1.0f)
        , rot(0.0f)
    {
    }

    engine::vec2 pos{};
    engine::vec2 scale{};
    engine::f32 rot{0};
};

struct Rectangle {
    Rectangle(engine::f32 w, engine::f32 h)
        : size(w, h)
    {
    }

    engine::vec2 size{};
};

struct Text {
    Text(engine::cstr text, engine::u32 size)
        : text(text)
        , size(size)
    {
    }

    engine::cstr text;
    engine::u32 size;
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

    ::Color color;
};

} // namespace components

using Entity = engine::ecs::
    Entity<components::Camera, components::Transform, components::Rectangle, components::Color, components::Text>;
using EntityStorage = engine::ecs::EntityStorage<Entity>;
using EntityBuilder = engine::ecs::EntityBuilder<Entity>;
using System        = engine::ecs::System<Entity>;
using SystemManager = engine::ecs::SystemManager<System>;

class SetupSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create().with<components::Camera>(0.0f, 0.0f).build();

        for (int i = 0; i < 1000; ++i) {
            float y = GetRandomValue(0, 450 - 10);
            float x = GetRandomValue(0, 800 - 10);
            components::Color color(GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255));

            builder.create()
                .with<components::Rectangle>(10.0f, 10.0f)
                .with<components::Color>(color)
                .with<components::Transform>(x, y)
                .build();
        }

        builder.create()
            .with<components::Text>("Congrats! You created your first window!", 20)
            .with<components::Color>(BLACK)
            .with<components::Transform>(190.0f, 200.0f)
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        float y = GetRandomValue(0, 450 - 10);
        float x = GetRandomValue(0, 800 - 10);
        components::Color color(GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255));

        builder.create()
            .with<components::Rectangle>(10.0f, 10.0f)
            .with<components::Color>(color)
            .with<components::Transform>(x, y)
            .build();

        engine::ecs::EntityId id = GetRandomValue(0, storage.size() - 1);

        if (storage.get(id).contains<components::Rectangle>()) {
            storage.remove(id);
        }
    }
};

class RenderSystem : public System
{
public:
    void update(Storage& storage) noexcept override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        const auto& [camera] = storage.get<components::Camera>();
        BeginMode2D(
            (Camera2D){.target = (Vector2){camera.pos.x, camera.pos.y}, .rotation = camera.rot, .zoom = camera.zoom});

        auto rect_iter = storage.iterator<components::Rectangle, components::Transform, components::Color>();
        while (rect_iter) {
            const auto& [rectangle, transform, color] = *rect_iter;

            DrawRectanglePro(
                (Rectangle){transform.pos.x, transform.pos.y, rectangle.size.x, rectangle.size.y},
                (Vector2){0.0f, 0.0f},
                transform.rot,
                color.color);

            ++rect_iter;
        }

        auto text_iter = storage.iterator<components::Text, components::Transform, components::Color>();
        while (text_iter) {
            const auto& [text, transform, color] = *text_iter;

            DrawText(text.text, transform.pos.x, transform.pos.y, text.size, color.color);

            ++text_iter;
        }

        EndMode2D();
        EndDrawing();
    }
};

class Game : public engine::Game
{
public:
    Game()
        : engine::Game(800, 450, "You Not Gonna Sleep Well Today")
    {
    }

    void setup() noexcept override
    {
        engine::Game::setup();

        manager_.add(std::make_unique<SetupSystem>());
        manager_.add(std::make_unique<RenderSystem>());
    }

    void update() noexcept override
    {
        manager_.update();
    }

    void shutdown() noexcept override
    {
        engine::Game::shutdown();
    }

private:
    SystemManager manager_;
};
} // namespace impl


int main(void)
{
    engine::uptr<engine::IGame> game    = std::make_unique<impl::Game>();
    engine::uptr<engine::Runner> runner = std::make_unique<engine::Runner>(std::move(game));
    runner->run();

    return 0;
}
