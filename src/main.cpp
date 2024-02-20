#include "box2d/box2d.h"
#include "raylib.h"

#include "engine/core.hpp"
#include "engine/defines.hpp"
#include "engine/ecs.hpp"

#include <iostream>

namespace impl
{
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

        using Entity        = engine::ecs::Entity<engine::vec2, engine::vec3>;
        using EntityStorage = engine::ecs::EntityStorage<Entity>;
        using EntityBuilder = engine::ecs::EntityBuilder<Entity>;

        EntityStorage storage;
        EntityBuilder builder(storage);

        builder.create().with<engine::vec2>(0.1, 3.5).with<engine::vec3>(0.1, 3.5, 1.0).build();
        builder.create().with<engine::vec2>(4.1, 1.5).build();
        builder.create().with<engine::vec3>(1.1, 2.5, 1.4).build();

        auto iter = storage.iterator<engine::vec2>();

        while (iter) {
            const auto& [a] = *iter;

            std::cout << a.x << " " << a.y << std::endl;

            ++iter;
        }
    }

    void update() noexcept override
    {
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
};
} // namespace impl


int main(void)
{
    engine::uptr<engine::IGame> game    = std::make_unique<impl::Game>();
    engine::uptr<engine::Runner> runner = std::make_unique<engine::Runner>(std::move(game));
    runner->run();

    return 0;
}
