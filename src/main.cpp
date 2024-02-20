#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"

#include <iostream>

namespace impl
{

using Entity        = engine::ecs::Entity<engine::vec2, engine::vec3>;
using EntityStorage = engine::ecs::EntityStorage<Entity>;
using EntityBuilder = engine::ecs::EntityBuilder<Entity>;
using System        = engine::ecs::System<Entity>;
using SystemManager = engine::ecs::SystemManager<System>;

class MainSystem : public System
{
public:
    void setup(Storage& storage) noexcept override
    {
        EntityBuilder builder(storage);

        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                builder.create().with<engine::vec2>(j, i).build();
            }
        }
    }
};

class PrintSystem : public System
{
public:
    void update(Storage& storage) noexcept override
    {
        auto iter = storage.iterator<engine::vec2>();

        while (iter) {
            const auto& [a] = *iter;
            std::cout << a.x << " " << a.y << std::endl;
            ++iter;
        }

        std::cout << "END" << std::endl;
    }
};

class RemoveSystem : public System
{
public:
    void update(Storage& storage) noexcept override
    {
        auto iter = storage.iterator<engine::vec2>();

        if (iter) {
            storage.remove(iter.id());
        }
    }
};

class AddSystem : public System
{
public:
    void update(Storage& storage) noexcept override
    {
        if (storage.active() == 0) {
            EntityBuilder builder(storage);

            for (int i = 0; i < 10; ++i) {
                for (int j = 0; j < 10; ++j) {
                    builder.create().with<engine::vec2>(j, i).build();
                }
            }
        }
    }
};


class Game : public engine::Game
{
public:
    Game()
        : engine::Game(800, 450, "You Not Gonna Dleep Well Today")
    {
    }

    void setup() noexcept override
    {
        engine::Game::setup();

        manager_.add(std::make_unique<MainSystem>());
        manager_.add(std::make_unique<PrintSystem>());
        manager_.add(std::make_unique<RemoveSystem>());
        manager_.add(std::make_unique<AddSystem>());
    }

    void update() noexcept override
    {
        manager_.update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
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
