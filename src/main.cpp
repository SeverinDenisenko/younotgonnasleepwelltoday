#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"

#include <cmath>

#include <fmt/format.h>

namespace impl
{

namespace components
{

struct Camera {
    Camera(engine::f32 x, engine::f32 y)
        : pos(x, y)
    {
    }

    Camera(engine::f32 x, engine::f32 y, engine::f32 zoom)
        : pos(x, y)
        , zoom(zoom)
    {
    }

    Camera() = default;

    engine::vec2 pos{0.0, 0.0};
    engine::f32 zoom{1};
    engine::f32 rot{0};
};

struct Transform {
    Transform(engine::f32 x, engine::f32 y)
        : pos(x, y)
    {
    }

    Transform() = default;

    engine::vec2 pos{0.0, 0.0};
    engine::vec2 scale{1.0, 1.0};
    engine::f32 rot{0};
};

struct Rectangle {
    Rectangle(engine::f32 w, engine::f32 h)
        : size(w, h)
    {
    }

    Rectangle() = default;

    engine::vec2 size{0.0, 0.0};
};

struct Text {
    Text(engine::cstr text, engine::f32 size, engine::f32 hspacing, engine::f32 wspacing)
        : text(text)
        , size(size)
        , hspacing(hspacing)
        , wspacing(wspacing)
    {
    }

    engine::cstr text;
    engine::f32 size;
    engine::f32 hspacing;
    engine::f32 wspacing;
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

struct Player {
};

} // namespace components

using Entity = engine::ecs::Entity<
    components::Camera,
    components::Transform,
    components::Rectangle,
    components::Color,
    components::Text,
    components::Player>;
using EntityStorage = engine::ecs::EntityStorage<Entity>;
using EntityBuilder = engine::ecs::EntityBuilder<Entity>;
using System        = engine::ecs::System<Entity>;
using SystemManager = engine::ecs::SystemManager<System>;

class DebugSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Text>("Placeholder", 3.0, 1.0, 3.0)
            .with<components::Color>(RED)
            .with<components::Transform>(1.0f, 1.0f)
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        fps_ = fmt::format("FPS: {} \nActive: {} \nAll: {}", GetFPS(), storage.active(), storage.size());

        const auto& [text] = storage.get<components::Text>();
        text.text          = fps_.data();
    }

private:
    std::string fps_;
};

class PlayerSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Player>()
            .with<components::Color>(RED)
            .with<components::Transform>()
            .with<components::Rectangle>(3.0f, 3.0f)
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        engine::f32 dt    = GetFrameTime();
        engine::f32 speed = 10.0f;

        const auto& [player, transform] = storage.get<components::Player, components::Transform>();

        if (IsKeyDown(KEY_W)) {
            transform.pos.y -= dt * speed;
        }
        if (IsKeyDown(KEY_S)) {
            transform.pos.y += dt * speed;
        }
        if (IsKeyDown(KEY_A)) {
            transform.pos.x -= dt * speed;
        }
        if (IsKeyDown(KEY_D)) {
            transform.pos.x += dt * speed;
        }
    }
};

class CellSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        engine::u32 size   = 100;
        engine::f32 square = 1.0f;

        for (engine::u32 i = 0; i < size; ++i) {
            for (engine::u32 j = 0; j < size; ++j) {
                engine::f32 x = i * square;
                engine::f32 y = j * square;

                engine::f32 u   = sin(x / size * engine::pi);
                engine::f32 v   = sin(y / size * engine::pi);
                engine::u8 grad = u * v * 255.0;
                components::Color color(grad, grad, grad);

                builder.create()
                    .with<components::Color>(color)
                    .with<components::Transform>(x, y)
                    .with<components::Rectangle>(square, square)
                    .build();
            }
        }
    }

    void update(Storage&) noexcept override {}

private:
    std::string fps_;
};

class RenderSystem : public System
{
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

        builder.create().with<components::Camera>(0.0f, 0.0f, h_ / 2.0f / view_).build();
    }

    void update(Storage& storage) noexcept override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        const auto& [camera] = storage.get<components::Camera>();
        BeginMode2D(
            (Camera2D){.target = (Vector2){camera.pos.x, camera.pos.y}, .rotation = camera.rot, .zoom = camera.zoom});

        rectangles(storage);
        text(storage);

        EndMode2D();
        EndDrawing();
    }

private:
    void rectangles(Storage& storage)
    {
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
    }

    void text(Storage& storage)
    {
        auto text_iter = storage.iterator<components::Text, components::Transform, components::Color>();

        while (text_iter) {
            const auto& [text, transform, color] = *text_iter;

            SetTextLineSpacing(text.wspacing);
            DrawTextPro(
                GetFontDefault(),
                text.text,
                (Vector2){transform.pos.x, transform.pos.y},
                (Vector2){0.0f, 0.0f},
                transform.rot,
                text.size,
                text.hspacing,
                color.color);

            ++text_iter;
        }
    }

    engine::u32 w_{0};
    engine::u32 h_{0};
    engine::f32 view_{0};
};

class Game : public engine::Game
{
public:
    Game()
        : engine::Game("You Not Gonna Sleep Well Today")
    {
    }

    void setup() noexcept override
    {
        engine::Game::setup();

        manager_.add(std::make_unique<CellSystem>());
        manager_.add(std::make_unique<PlayerSystem>());
        manager_.add(std::make_unique<DebugSystem>());
        manager_.add(std::make_unique<RenderSystem>(width(), height(), 100.0f));
    }

    void update() noexcept override
    {
        manager_.update();

        if (IsKeyPressed(KEY_ESCAPE)) {
            exit();
        }
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
