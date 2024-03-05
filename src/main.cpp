#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"
#include "engine/profiling.hpp"

#include <cmath>

#include <fmt/format.h>

namespace impl
{

namespace components
{

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

class TransformBuilder
{
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

struct Rectangle {
    Rectangle(engine::f32 w, engine::f32 h)
        : size(w, h)
    {
    }

    Rectangle() = default;

    engine::vec2 size{0.0f, 0.0f};
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

class PlayerSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        builder.create()
            .with<components::Player>()
            .with<components::Color>(RED)
            .with<components::Transform>(components::TransformBuilder().create().origin(1.5f, 1.5f).build())
            .with<components::Rectangle>(3.0f, 3.0f)
            .build();
    }

    void update(Storage& storage) noexcept override
    {
        engine::f32 dt    = GetFrameTime();
        engine::f32 speed = 10.0f;

        const auto& [player, ptransform] = storage.get<components::Player, components::Transform>();
        const auto& [camera, ctransform] = storage.get<components::Camera, components::Transform>();

        if (IsKeyDown(KEY_W)) {
            ptransform.pos.y -= dt * speed;
            ctransform.pos.y -= dt * speed;
        }
        if (IsKeyDown(KEY_S)) {
            ptransform.pos.y += dt * speed;
            ctransform.pos.y += dt * speed;
        }
        if (IsKeyDown(KEY_A)) {
            ptransform.pos.x -= dt * speed;
            ctransform.pos.x -= dt * speed;
        }
        if (IsKeyDown(KEY_D)) {
            ptransform.pos.x += dt * speed;
            ctransform.pos.x += dt * speed;
        }
    }
};

class CellSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        engine::u32 size   = 600;
        engine::f32 square = 100.0f / size;

        for (engine::u32 i = 0; i < size; ++i) {
            for (engine::u32 j = 0; j < size; ++j) {
                engine::f32 x = i * square;
                engine::f32 y = j * square;

                engine::f32 u   = pow(sin(engine::f32(i) / engine::f32(size) * engine::pi * 2.0f), 2.0f);
                engine::f32 v   = pow(sin(engine::f32(j) / engine::f32(size) * engine::pi * 2.0f), 2.0f);
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

        const auto& [camera, transform] = storage.get<components::Camera, components::Transform>();
        BeginMode2D((Camera2D){
            .target   = (Vector2){transform.pos.x, transform.pos.y},
            .offset   = (Vector2){transform.origin.x, transform.origin.y},
            .rotation = transform.rot,
            .zoom     = camera.zoom});

        rectangles(storage);
        text(storage);

        EndMode2D();
        EndDrawing();
    }

private:
    void rectangles(Storage& storage)
    {
        PROFILE_FUNCTION();

        auto rect_iter = storage.iterator<components::Rectangle, components::Transform, components::Color>();

        while (rect_iter) {
            const auto& [rectangle, transform, color] = *rect_iter;

            DrawRectanglePro(
                (Rectangle){transform.pos.x, transform.pos.y, rectangle.size.x, rectangle.size.y},
                (Vector2){transform.origin.x, transform.origin.y},
                transform.rot,
                color.color);

            ++rect_iter;
        }
    }

    void text(Storage& storage)
    {
        PROFILE_FUNCTION();

        auto text_iter = storage.iterator<components::Text, components::Transform, components::Color>();

        while (text_iter) {
            const auto& [text, transform, color] = *text_iter;

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
        PROFILE_FUNCTION();

        engine::Game::setup();

        manager_.add(std::make_unique<RenderSystem>(width(), height(), 100.0f));
        manager_.add(std::make_unique<CellSystem>());
        manager_.add(std::make_unique<PlayerSystem>());
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
