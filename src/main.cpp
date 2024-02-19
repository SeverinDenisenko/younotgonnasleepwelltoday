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

        engine::ecs::Entity<engine::vec2, engine::vec3> ent;
        std::cout << ent.size() << " " << ent.count() << std::endl;
        std::cout << ent.index<engine::vec2>() << " " << ent.index<engine::vec3>() << std::endl;
        std::cout << ent.size<engine::vec2>() << " " << ent.size<engine::vec3>() << std::endl;

        std::cout << ent.contains<engine::vec2>() << " " << ent.contains<engine::vec3>() << std::endl;

        ent.add<engine::vec2>(engine::vec2{0.0f, 1.0f});

        std::cout << ent.contains<engine::vec2>() << " " << ent.contains<engine::vec3>() << std::endl;

        ent.remove<engine::vec2>();
        ent.add<engine::vec3>(engine::vec3{0.0f, 1.0f, 2.0f});

        std::cout << ent.contains<engine::vec2>() << " " << ent.contains<engine::vec3>() << std::endl;

        engine::vec3& res = ent.get<engine::vec3>();

        std::cout << res.x << " " << res.y << " " << res.z << std::endl;

        std::cout << ent.contains<engine::vec2, engine::vec3>() << std::endl;

        ent.add<engine::vec2>(engine::vec2{0.0f, 1.0f});

        std::cout << ent.contains<engine::vec2, engine::vec3>() << std::endl;
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
